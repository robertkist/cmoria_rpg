#ifndef ENGINE_BITMAP_H
#define ENGINE_BITMAP_H

#include "helpers/h_coordinate.h"

#ifdef __cplusplus
extern "C" {
#endif

struct EngineBitmap {
    void* bitmap;
    int width;
    int height;
};

struct EngineBitmap* EngineBitmap_new(const char* fname);
const void* EngineBitmap_getBitmap(const struct EngineBitmap* self);
void EngineBitmap_delete(struct EngineBitmap* self);

#ifdef __cplusplus
}
#endif

#endif