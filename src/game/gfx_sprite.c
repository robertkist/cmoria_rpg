#include <stdio.h>
#include "gfx_sprite.h"
#include "helpers/h_memory.h"
#include "engine.h"

struct GfxSprite* GfxSprite_new(const char* fname, uint8_t maxFrames, unsigned long currentTime) {
    struct GfxSprite* self = (struct GfxSprite*)GMalloc(sizeof(struct GfxSprite), "GfxSprite");
    self->frames = (struct EngineBitmap**)GMalloc(sizeof(struct EngineBitmap*) * maxFrames, "GfxSprite->frames");
    if (fname != NULL) {
        self->frames[0] = EngineBitmap_new(fname);
        self->frameCount = 1;
    } else {
        self->frameCount = 0;
    }
    if (maxFrames > 1)
        self->durations = (int*)GMalloc(sizeof(int) * maxFrames, "GfxSprite->durations");
    self->maxFrames = maxFrames;
    self->currentFrame = 0;
    self->manualAdvance = false;

    self->lastFrameTime = currentTime;
    return self;
}

void GfxSprite_addFrame(struct GfxSprite* self, const char* fname, int duration) {
//    printf("framecount %i %i %s\n", self->frameCount, self->maxFrames, fname);
    if (self->frameCount < self->maxFrames) {
        self->frames[self->frameCount] = EngineBitmap_new(fname);
        self->durations[self->frameCount] = duration;
        self->frameCount++;
    } else {
        Engine_logError("GfxSprite_addFrame - too many frames!");
        Engine_exit(-1);
    }
}

bool GfxSprite_advanceFrame(struct GfxSprite* self, unsigned long currentTime) {
    if (self->frameCount > 1 && !self->manualAdvance) {
        if (Engine_timeDiff(currentTime, self->lastFrameTime) > (unsigned long)self->durations[self->currentFrame]) {
            self->lastFrameTime = currentTime;
            self->currentFrame++;
            if (self->currentFrame == self->frameCount) {
                self->currentFrame = 0;
            }
            return true;
        }
    }
    return false;
}

void GfxSprite_delete(struct GfxSprite* self) {
    if (self) {
        for (uint8_t i = 0; i < self->frameCount; i++) {
            if (self->frames[i])
                EngineBitmap_delete(self->frames[i]);
        }
        if (self->maxFrames > 1)
            GFree(self->durations, "GfxSprite->durations");
        GFree(self->frames, "GfxSprite->frames");
        GFree(self, "GfxSprite");
    }
}

const void* GfxSprite_getBitmap(const struct GfxSprite* self) {
    return self->frames[self->currentFrame];
}

const void* GfxSprite_getFrame(const struct GfxSprite* self, int frame) {
    return self->frames[frame];
}