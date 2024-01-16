#ifndef GFX_SPRITE_H
#define GFX_SPRITE_H

#include <stdint.h>
#include <stdbool.h>
#include "engine_bitmap.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GfxSprite {
    struct EngineBitmap** frames;
    uint8_t currentFrame;
    uint8_t frameCount;
    uint8_t maxFrames;
    int* durations;
    bool manualAdvance;
    unsigned long lastFrameTime;
};

struct GfxSprite* GfxSprite_new(const char* fname, uint8_t maxFrames, unsigned long currentTime);
void GfxSprite_delete(struct GfxSprite* self);
void GfxSprite_addFrame(struct GfxSprite* self, const char* fname, int duration);
const void* GfxSprite_getBitmap(const struct GfxSprite* self);
const void* GfxSprite_getFrame(const struct GfxSprite* self, int frame);
bool GfxSprite_advanceFrame(struct GfxSprite* self, unsigned long currentTime);

#ifdef __cplusplus
}
#endif

#endif