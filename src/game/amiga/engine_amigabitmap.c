#include "engine_amigabitmap.h"

#define SCR_DEPTH 2

extern struct Screen *screen;

struct BitMap* CreateShadowBM(SHORT depth, SHORT width, SHORT height);
struct RastPort* CreateShadowRP(struct BitMap* shadowBM);

int ClipBlitTransparent(struct RastPort* srp, SHORT sx, SHORT sy,
                        struct RastPort* drp, SHORT dx, SHORT dy,
                        SHORT width, SHORT height,
                        struct RastPort* shadowRP) {
    /* USE SHADOW MASK TO BLAST A HOLE IN THE DESTINATION AREA */
    ClipBlit(shadowRP, 0, 0, drp, dx, dy, width, height, 0x20);
    /* FILL THE HOLE WITH SOURCE MATERIAL */
    ClipBlit(srp, sx, sy, drp, dx, dy, width, height, 0xE0);
    return(0);
}

struct BitMap* createAmigaBitmap(int depth, int width, int height) {
    struct BitMap* bmp = (struct BitMap*)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
    if (bmp == NULL)
        return NULL;
    InitBitMap(bmp, depth, width, height);
    for (int i = 0; i < depth; i++) {
        bmp->Planes[i] = (PLANEPTR)AllocRaster(width, height);
        if (bmp->Planes[i] == NULL ) {
            freeAmigaBitmap(bmp);
            return NULL;
        }
        BltClear(bmp->Planes[i], (width / 8) * height, 1);
    }
    return bmp;
}

void freeAmigaBitmap(struct BitMap* bmp) {
    if (bmp != NULL) {
        for (int i = 0; i < bmp->Depth; i++) {
            if (bmp->Planes[i] != NULL)
                FreeRaster(bmp->Planes[i], bmp->BytesPerRow * 8, bmp->Rows);
        }
        FreeMem(bmp, (LONG)sizeof(struct BitMap));
    }
}

struct AmigaBitmap* AmigaBitmap_load(const char* fname) {
    int16_t bmpWidth = 0;
    int16_t bmpHeight = 0;
    int8_t bmpDepth = 0;
    int8_t transparency = 0;

    FILE* fp = fopen(fname,"rb");
    fread(&bmpWidth, sizeof(int16_t), 1, fp);
    fread(&bmpHeight, sizeof(int16_t), 1, fp);
    fread(&bmpDepth, sizeof(int8_t), 1, fp);
    fread(&transparency, sizeof(int8_t), 1, fp);

    size_t size = bmpWidth / 8 * bmpHeight;
    struct BitMap* bmp = createAmigaBitmap(bmpDepth, bmpWidth, bmpHeight);
    char* buffer = (char*)calloc(size, 1);
    for (int i = 0; i < bmpDepth; i++) {
        fread(buffer, size, 1, fp);
        memcpy(bmp->Planes[i], buffer, size);
    }

    struct AmigaBitmap* amigaBmp = (struct AmigaBitmap*)calloc(sizeof(struct AmigaBitmap), 1);
    amigaBmp->width = bmpWidth;
    amigaBmp->height = bmpHeight;
    amigaBmp->bmp = bmp;
    amigaBmp->rowBytes = amigaBmp->bmp->BytesPerRow * 8;

    amigaBmp->rp = (struct RastPort*)AllocMem(sizeof(struct RastPort), MEMF_CHIP);
    InitRastPort(amigaBmp->rp);
    amigaBmp->rp->BitMap = amigaBmp->bmp;

    if (transparency == 1) {
        amigaBmp->mask = CreateShadowBM(SCR_DEPTH, amigaBmp->width, amigaBmp->height); // shadow (aka mask) bitmap
        amigaBmp->maskRp = CreateShadowRP(amigaBmp->mask); // create mask
        fread(buffer, size, 1, fp);
        memcpy(amigaBmp->mask->Planes[0], buffer, size);
    }

    free(buffer);
    fclose(fp);
    return amigaBmp;
}

void AmigaBitmap_delete(struct AmigaBitmap* self) {
    if (self) {
        freeAmigaBitmap(self->bmp);
        FreeMem(self->rp, sizeof(struct RastPort));
        if (self->mask) {
            freeAmigaBitmap(self->mask);
            FreeMem(self->maskRp, sizeof(struct RastPort));
        }
        free(self);
    }
}

void AmigaBitmap_draw(struct AmigaBitmap* self, int x, int y) {
    int bx = 0;
    int by = 0;
    int width = self->width;
    int height = self->height;
    int xwidth = x + width;
    int yheight = y + height;
    bool clip = false;
    if (xwidth < 0 || yheight < 0 || x >= 320 || y >=256)
        return; // bitmap is entirely out of screen
    if (x < 0) { bx = width - xwidth; width = xwidth; x = 0; clip = true; }
    else if (xwidth >= 320) { width -= xwidth - 320; clip = true; }
    if (width == 0)
        return;
    if (y < 0) { by = height - yheight; height = yheight; y = 0; clip = true; }
    else if (yheight >= 256) { height -= yheight - 256; clip = true; }
    if (height == 0)
        return;
    if (self->mask) {
        ClipBlit(self->maskRp, 0, 0, &(screen->RastPort), x, y, width, height, 0x20);
        ClipBlit(self->rp, bx, by, &(screen->RastPort), x, y, width, height, 0xE0);
    } else if (clip) {
        ClipBlit(self->rp, bx, by,
                 &(screen->RastPort), x, y,
                 width, height,
                 0x0040);
    } else {
        BltBitMapRastPort(self->bmp, 0, 0,
                          &(screen->RastPort), x, y,
                          self->rowBytes,
                          self->bmp->Rows, 0x0040);
    }
}

struct BitMap* CreateShadowBM(SHORT depth, SHORT width, SHORT height) {
    /* Shadow mask is one bitplane deep, and only as wide and high as the area we want
     * to move. It gets one plane pointer worth of memory. BUT it LOOKS like a full 5
     * or so planes deep. */
    struct BitMap *shadowBM;
    if((shadowBM = (struct BitMap *)AllocMem(sizeof(struct BitMap), MEMF_CHIP)) == NULL )
        return(NULL);
    InitBitMap(shadowBM,depth, width,height);
    if((shadowBM->Planes[0] = (PLANEPTR)AllocMem(RASSIZE(width,height), MEMF_CHIP | MEMF_CLEAR)) == NULL) {
        FreeMem(shadowBM, sizeof(struct BitMap));
        return (NULL);
    }
    for(int i = 1; i < depth; i++) {
        shadowBM->Planes[i] = shadowBM->Planes[0]; // point all other planes to first plane
    }
    return (shadowBM);
}

struct RastPort* CreateShadowRP(struct BitMap* shadowBM) {
    // creates a rastport
    struct RastPort *shadowRP;
    if ((shadowRP = (struct RastPort*)AllocMem(sizeof(struct RastPort), MEMF_CHIP)) == NULL)
        return (NULL);
    InitRastPort(shadowRP);
    shadowRP->BitMap = shadowBM; // link together the bitmap and the rastport */
    return(shadowRP);
}