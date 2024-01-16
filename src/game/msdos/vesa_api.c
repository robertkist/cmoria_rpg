#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <time.h>
#include <sys/nearptr.h>
#include "engine_bitmap_msdos.h"

#define XOFS 100
#define YOFS 70

typedef struct {
    /* SVGA Mode Information */
    uint16_t   ModeAttributes ;         /* Mode attributes                  */
    uint8_t    WinAAttributes ;         /* Window A attributes              */
    uint8_t    WinBAttributes ;         /* Window B attributes              */
    uint16_t   WinGranularity ;         /* Window granularity in k          */
    uint16_t   WinSize ;                /* Window size in k                 */
    uint16_t   WinASegment ;            /* Window A segment                 */
    uint16_t   WinBSegment ;            /* Window B segment                 */
    intptr_t  *WinFuncPtr ;             /* Pointer to window function       */
    uint16_t   BytesPerScanLine ;       /* Bytes per scanline               */
    uint16_t   XResolution ;            /* Horizontal resolution            */
    uint16_t   YResolution ;            /* Vertical resolution              */
    uint8_t    XCharSize ;              /* Character cell width             */
    uint8_t    YCharSize ;              /* Character cell height            */
    uint8_t    NumberOfPlanes ;         /* Number of memory planes          */
    uint8_t    BitsPerPixel ;           /* Bits per pixel                   */
    uint8_t    NumberOfBanks ;          /* Number of CGA style banks        */
    uint8_t    MemoryModel ;            /* Memory model type                */
    uint8_t    BankSize ;               /* Size of CGA style banks          */
    uint8_t    NumberOfImagePages ;     /* Number of images pages           */
    uint8_t    res1 ;                   /* Reserved                         */
    uint8_t    RedMaskSize ;            /* Size of direct color red mask    */
    uint8_t    RedFieldPosition ;       /* Bit posn of lsb of red mask      */
    uint8_t    GreenMaskSize ;          /* Size of direct color green mask  */
    uint8_t    GreenFieldPosition ;     /* Bit posn of lsb of green mask    */
    uint8_t    BlueMaskSize ;           /* Size of direct color blue mask   */
    uint8_t    BlueFieldPosition ;      /* Bit posn of lsb of blue mask     */
    uint8_t    RsvdMaskSize ;           /* Size of direct color res mask    */
    uint8_t    RsvdFieldPosition ;      /* Bit posn of lsb of res mask      */
    uint8_t    DirectColorModeInfo ;    /* Direct color mode attributes     */
    /* VESA 2.0 variables */
    uint32_t   PhysBasePtr;             /* physical address for flat frame buffer */
    uint32_t   OffScreenMemOffset;		/* pointer to start of off screen memory */
    uint16_t   OffScreenMemSize;	    /* amount of off screen memory in 1k units */
    uint8_t    res2[206] ;              /* Pad to 256 byte block size       */
} VBE_ModeInfo;

struct VESA_Info {
    uint8_t  VESASignature[4];
    uint16_t VESAVersion;
    uint32_t OEMStringPtr;
    uint32_t Capabilities;
    uint32_t VideoModePtr;
    uint16_t TotalMemory;
    uint16_t OemSoftwareRev;
    uint32_t OemVendorNamePtr;
    uint32_t OemProductNamePtr;
    uint32_t OemProductRevPtr;
    uint8_t  Reserved[222];
    uint8_t  OemData[256];
} __attribute__ ((packed));


__dpmi_meminfo mapping;
uint8_t *video = NULL; // pointer to screen framebuffer
int xres, yres;        // screen resolution
bool wide_DAC = false; // wide DAC only works on dosbox-x
size_t vmemBankSize;
bool pageFlipToggle = false;
VESA_Info vesainfo;


int VBE_set_vesa_mode() {
    /* get video card info */
    assert(sizeof(vesainfo) < _go32_info_block.size_of_transfer_buffer);
    __dpmi_regs regs;
    regs.x.ax = 0x4F00;
    regs.x.di = __tb & 0x0F;
    regs.x.es = (__tb >> 4) & 0xFFFF;
    __dpmi_int(0x10, &regs);
    dosmemget(__tb, sizeof(vesainfo), &vesainfo);
    if (vesainfo.TotalMemory < 8) { // less than 512kb (8 * 64 = 512)
        return -1; // not enough VRAM
    }
    /* Sets VESA Super-VGA video mode 640 x 480 x 8bpp (256 colors) */
    uint16_t mode = 0x100;  // 640 x 480 x 8bpp (256 colors) w. double buffering support (needs 512 vram)
    /* get info about the chosen video mode */
    VBE_ModeInfo modeinfo;
    assert(sizeof(modeinfo) < _go32_info_block.size_of_transfer_buffer);
    __dpmi_regs regsa;
    regsa.x.ax = 0x4F01;
    regsa.x.cx = mode;
    regsa.x.di = __tb & 0x0F;
    regsa.x.es = (__tb >> 4) & 0xFFFF;
    __dpmi_int(0x10, &regsa);
    dosmemget(__tb, sizeof(modeinfo), &modeinfo);
    /* prepare VRAM memory mapping */
    xres = modeinfo.XResolution;
    yres = modeinfo.YResolution;
    uint16_t bpl;
    uint16_t bpp = modeinfo.BitsPerPixel / 8;
    bpl = xres * bpp;
    mapping.address = modeinfo.PhysBasePtr;
    vmemBankSize = yres * xres * (bpp);
    mapping.size = vmemBankSize * 2;
    if (__dpmi_physical_address_mapping(&mapping) != 0) {
        return -1;
    }
    /* set the video mode */
    __dpmi_regs regsb;
    regsb.x.ax = 0x4F02;
    regsb.x.bx = 0x4000 + mode;
    __dpmi_int(0x10, &regsb);
    if (regsb.x.ax != 0x4F) {
        return -1;
    }
    /* switch to 8-bit DAC */
    __dpmi_regs regsc;
    regsc.x.ax = 0x4F08;
    regsc.x.bx = 0x0800;      // set to 0x0600 for explicitly requesting a 6-bit DAC
    __dpmi_int(0x10, &regsc);
    if (regsc.x.ax == 0x4F) { // Function succeeded
        wide_DAC = true;      // set 8 bit DAC
    }
    if(regsc.h.bh == 6) { // verify if DAC has changed
        wide_DAC = false; // DAC is still 6 bit wide
    }
    /* provide pointer to VRAM */
    video = (uint8_t *)(mapping.address + __djgpp_conventional_base);
    return 0;
}

void VBE_set_palette_entry(uint8_t entry, uint8_t r, uint8_t g, uint8_t b) {
    outportb(0x3c8, entry);
    if (wide_DAC) {
        outportb(0x3c9, r);
        outportb(0x3c9, g);
        outportb(0x3c9, b);
    } else {
        outportb(0x3c9, r >> 2);
        outportb(0x3c9, g >> 2);
        outportb(0x3c9, b >> 2);
    }
}

void VBE_set_palette(const struct GfxBitmap* palette) {
    for (int i = 0; i < palette->colors; i++) {
        int j = i * 4;
        VBE_set_palette_entry(palette->palette[j],
                              palette->palette[j + 1],
                              palette->palette[j + 2],
                              palette->palette[j + 3]);
    }
}

void VBE_set_text_mode() {
    /* switches the video mode back to text mode */
    __dpmi_regs regs;
    regs.x.ax = 0x4F02;
    regs.x.bx = 0x3;
    __dpmi_int(0x10, &regs);
}

void VBE_draw_rect(int x1, int y1, int width, int height) {
    size_t offset = pageFlipToggle ? 0 : vmemBankSize;
    uint8_t* ofs1 = video + offset + ((y1 + YOFS) * xres) + XOFS;
    uint8_t* ofs2 = video + offset + ((y1 + YOFS + height - 1) * xres) + XOFS;
    int x2 = x1 + width - 1;
    for (int x = x1; x < x2; x++) {
        *(ofs1 + x) = 5;
        *(ofs2 + x) = 5;
    }
    int y2 = y1 + height - 1;
    ofs1 = video + offset + ((y1 + YOFS) * xres) + x1 + XOFS;
    for (int y = y1; y <= y2; y++) {
        *ofs1 = 5;
        *(ofs1 + width - 1) = 5;
        ofs1 += xres;
    }
}

void VBE_draw_bitmap(const struct GfxBitmap* bitmap, int dstx, int dsty) {
    uint8_t *buffer = GfxBitmap_getBitmap(bitmap);
    size_t offset = pageFlipToggle ? 0 : vmemBankSize;

    int width = bitmap->width;
    int height = bitmap->height;
    int xwidth = width + dstx + XOFS;
    int yheight = height + dsty + YOFS;

    if (xwidth < 0 || yheight < 0 || dstx >= 400 || dsty >=240)
        return; // bitmap is entirely out of screen

    uint8_t* ofs = video + offset + ((dsty + YOFS) * xres) + XOFS;
    ofs += dstx;
    int ystart = 0;
    if (xwidth > XOFS + 400) {
        width -= ((xwidth) - (XOFS + 400));
    }
    else if (dstx + XOFS < XOFS) {
        width -= (XOFS - (dstx + XOFS));
        int o = bitmap->width - width;
        ofs += o;
        buffer += o;
    }
    if (width == 0)
        return;

    if (yheight > YOFS + 240) {
        height -= ((yheight) - (YOFS + 240));
    }
    else if (dsty + YOFS < YOFS) {
        height -= (YOFS - (dsty + YOFS));
        int o = bitmap->height - height;
        ofs += (o * xres);
        buffer += (o * bitmap->width);
    }
    if (height == 0)
        return;

    if (bitmap->transparency) {
        int ofsInc = xres - width;
        int bufferInc = bitmap->width - width;
        for (int y = ystart; y < height; y++, ofs += ofsInc, buffer += bufferInc) {
            for (int x = 0; x < width; x++, buffer++, ofs++) {
                if (*buffer != 0)
                    *ofs = *buffer;
            }
        }
    } else {
        for (int y = ystart; y < height; y++, ofs += xres, buffer += bitmap->width) {
            memcpy(ofs, buffer, width);
        }
    }
}

void WaitForVBlank() {  // wait for vsync
    /* not needed when using double buffering */
    while (inp(0x3DA) & 8);
    while (!(inp(0x3DA) & 8));
}

void VBE_clear() {
//    size_t offset = pageFlipToggle ? 0 : vmemBankSize;
//    uint8_t* v = video + offset;
//    for (size_t i = 0; i < vmemBankSize; i++) {
//        *v = 0x2;
//        v++;
//    }
}

void VBE_flip() {
    __dpmi_regs regsb;
    regsb.x.ax = 0x4F07;
    regsb.x.bx = 0x80;
    regsb.x.cx = 0;
    regsb.x.dx = pageFlipToggle ? 0 : 400;
    __dpmi_int(0x10, &regsb);
    pageFlipToggle = !pageFlipToggle;
}