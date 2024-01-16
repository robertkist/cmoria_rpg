#include "helpers/h_memory.h"
#include "../../src/game/engine_bitmap.h"
#include "engine.h"

struct EngineBitmap* EngineBitmap_new(const char* fname) {
    Engine_log("EngineBitmap_new: '%s'", fname);
    struct EngineBitmap* self = (struct EngineBitmap*)GMalloc(sizeof(struct EngineBitmap), "EngineBitmap");
    Engine_loadBitmap(fname, self);
#ifdef DEBUG
    if (!self->bitmap) {
        Engine_logError("Cannot load bitmap");
        Engine_exit(-1);
    }
#endif
    return self;
}

const void* EngineBitmap_getBitmap(const struct EngineBitmap* self) {
    return self->bitmap;
}

void EngineBitmap_delete(struct EngineBitmap *self) {
    if (self) {
        Engine_freeBitmap(self->bitmap);
        GFree(self, "EngineBitmap");
    }
}
