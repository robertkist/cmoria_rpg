#ifndef ENGINE_AMIGABITMAP_H
#define ENGINE_AMIGABITMAP_H

// TODO not all includes are needed...

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>

// C includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct AmigaBitmap {
    struct BitMap* bmp;
    struct BitMap* mask;
    struct RastPort* rp;
    struct RastPort* maskRp;
    int rowBytes;
    int width;
    int height;
};

struct BitMap* createAmigaBitmap(int depth, int width, int height);
void freeAmigaBitmap(struct BitMap* bmp);

struct AmigaBitmap* AmigaBitmap_load(const char* fname);
void AmigaBitmap_draw(struct AmigaBitmap* self, int x, int y);
void AmigaBitmap_delete(struct AmigaBitmap* self);

#endif