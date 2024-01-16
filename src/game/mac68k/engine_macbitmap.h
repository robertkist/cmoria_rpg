#ifndef ENGINE_MACBITMAP_H
#define ENGINE_MACBITMAP_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <Quickdraw.h>

struct MacBitmap {
    BitMap* bitmap;
    BitMap* mask;
    bool transparency;
};

struct MacBitmap* MacBitmap_new(const char* fname);
void MacBitmap_delete(struct MacBitmap* self);
void MacBitmap_draw(struct MacBitmap* self, int ofsx, int ofsy);

#endif