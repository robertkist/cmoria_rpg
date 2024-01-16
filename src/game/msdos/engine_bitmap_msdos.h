#ifndef ENGINE_BITMAP_MSDOS_H
#define ENGINE_BITMAP_MSDOS_H

#include <stdbool.h>
#include <stdint.h>

struct GfxBitmap { // a bitmap with palette
    uint16_t width;
    uint16_t height;
    uint8_t *bitmap;
    uint8_t colors;
    uint8_t *palette;
    bool hasPalette;
    bool transparency;
};

uint16_t GfxBitmap_width(const GfxBitmap *self);
uint16_t GfxBitmap_height(const GfxBitmap *self);
GfxBitmap* GfxBitmap_new(const char fname[]);
uint8_t* GfxBitmap_getBitmap(const GfxBitmap *self);
void GfxBitmap_delete(GfxBitmap *self);

#endif
