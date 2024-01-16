#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "engine_bitmap_msdos.h"

//#define LOAD_PALETTE 1

const int MAX_BUF = 1000;
bool GfxBitmap_loadBitmap(GfxBitmap *self, const char fname[]);
bool GfxBitmap_loadPalette(GfxBitmap *self, const char fname[]);

GfxBitmap* GfxBitmap_new(const char fname[]) {
    /* Constructor for Bitmap object */
    GfxBitmap *self;
    self = (GfxBitmap*)calloc(1, sizeof(GfxBitmap));
    self->hasPalette = false;
    self->transparency = false;
    char* buffer = (char*)malloc(MAX_BUF);
    sprintf(buffer, "%s.bmp", fname);
    if (!GfxBitmap_loadBitmap(self, buffer))
        return NULL;
#ifdef LOAD_PALETTE
    sprintf(buffer, "%s.pal", fname);
    if (!GfxBitmap_loadPalette(self, buffer))
        return NULL;
#endif
    free(buffer);
    return self;
}

bool GfxBitmap_loadBitmap(GfxBitmap *self, const char fname[]) {
    /* load image */
    if(access(fname, F_OK) != 0) {
        return false;
    }
    FILE *fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp) - 5;
    fseek(fp, 0, SEEK_SET); /* same as rewind(f); */
    fread(&(self->width), 1, 2, fp);
    fread(&(self->height), 1, 2, fp);
    fread(&(self->transparency), 1, 1, fp);
    self->bitmap = (uint8_t*)calloc((unsigned long)fsize, 1);
    size_t r = fread(self->bitmap, 1, (unsigned long)fsize, fp);
    assert(r == fsize);
    fclose(fp);
    assert(self->bitmap && "GfxImage_new could not load image");
    return true;
}

bool GfxBitmap_loadPalette(GfxBitmap *self, const char fname[]) {
    /* load palette */
    if(access(fname, F_OK) != 0) {
        return false;
    }
    FILE *fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp) - 1;
    fseek(fp, 0, SEEK_SET); /* same as rewind(f); */
    fread(&(self->colors), 1, 1, fp);
    self->palette = (uint8_t*)calloc(self->colors * 4, 1); // 1 byte each: index, r, g, b
    fread(self->palette, 1, self->colors * 4, fp);
    fclose(fp);
    assert(self->palette && "GfxPalette_new could not load palette");
    self->hasPalette = true;
    return true;
}

uint16_t GfxBitmap_width(const GfxBitmap *self) {
    /* returns the bitmap's width */
    return self->width;
}

uint16_t GfxBitmap_height(const GfxBitmap *self) {
    /* returns the bitmap's height */
    return self->height;
}

uint8_t* GfxBitmap_getBitmap(const GfxBitmap *self) {
    /* returns the bitmap */
    return self->bitmap;
}

void GfxBitmap_delete(GfxBitmap *self) {
    /* Destructor for Bitmap object */
    if(self) {
        free(self->bitmap);
        free(self);
        self = NULL;
    }
}
