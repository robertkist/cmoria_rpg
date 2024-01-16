#ifndef VESA_API_H
#define VESA_API_H

int VBE_set_vesa_mode();
void VBE_set_text_mode();
void VBE_set_palette(const struct GfxBitmap* palette);
void VBE_draw_bitmap(const struct GfxBitmap* bitmap, int dstx, int dsty);
void VBE_draw_rect(int x1, int y1, int width, int height);
void VBE_set_palette_entry(uint8_t entry, uint8_t r, uint8_t g, uint8_t b);
void VBE_clear();
void VBE_flip();

#endif