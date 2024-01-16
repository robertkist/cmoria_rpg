#include <stdio.h>
#include "gfx_tiledb.h"
#include "engine.h"
#include "helpers/h_memory.h"
#include "engine_bitmap.h"
#include "helpers/h_ini_file.h"
#include "helpers/h_stringarray.h"
#include "gfx_sprite.h"

static struct GfxSprite** gfxSprites = NULL;
static size_t tileCount = 0;

size_t GfxTileDb_getTileCount(void) {
    return tileCount;
}

void GfxTileDb_loadTiles(const char* fname) {
    Engine_log("loading tiles from '%s'", fname);
    char* iniString = Engine_loadTextFile(fname);
    struct HIniFile* ini = HIniFile_new(iniString);
    GFree(iniString, "Engine_loadTextFile");
    tileCount = HIniFile_getInt(ini, "general", "count", 0);
#ifdef DEBUG
    if (tileCount == 0) {
        Engine_logError("no tiles found in '%s'", fname);
        Engine_exit(-1);
    }
#endif
    gfxSprites = (struct GfxSprite**)GMalloc(tileCount * sizeof(struct GfxSprite*), "GfxTileDb");

    unsigned long currentTime = Engine_getTime();
    for (size_t i = 0; i < tileCount; i++) {
        char section[40];
        snprintf(section, 40, "%li", (long int)i);

        int frameCount = HIniFile_getInt(ini, section, "frame_count", 1);
        char* image = HIniFile_get(ini, section, "image");
        if (frameCount == 1) {
            gfxSprites[i] = GfxSprite_new(image, (uint8_t)frameCount, currentTime);
        } else {
            gfxSprites[i] = GfxSprite_new(NULL, (uint8_t)frameCount, currentTime);
//            printf("     frame count %i\n", frameCount);
//            bool manualAdvance = HIniFile_getBool(ini, section, "manual_advance", true); // TODO
//            printf("     manual advance %i\n", manualAdvance);
            char* durationsStr = HIniFile_get(ini, section, "durations");
//            printf("     durations %s\n", durationsStr);
            char* framesStr = HIniFile_get(ini, section, "frames");
//            printf("     frames %s\n", framesStr);

            int* durations = (int*)GMalloc(sizeof(int) * (frameCount), "GfxTileDb_loadTiles->durations");
            csvStrToArray(durations, durationsStr, frameCount);
            struct StringArray* frames = StringArray_fromCsv(framesStr, frameCount - 1);
//            printf("     duration %i\n", durations[0]);
//            printf("     frame %s\n", image);
            GfxSprite_addFrame(gfxSprites[i], image, durations[0]);
            for(int g = 1; g < frameCount; g++) {
//                printf("     duration %i\n", durations[g]);
//                printf("     frame %s\n", frames->string[g - 1]);
                GfxSprite_addFrame(gfxSprites[i], frames->string[g - 1], durations[g]);
            }

            StringArray_delete(frames);
            GFree(durations, "GfxTileDb_loadTiles->durations");
        }
    }
    HIniFile_delete(ini);
}

const struct EngineBitmap* GfxTileDb_getBitmap(int id) {
#ifdef DEBUG
    if (id >= (int)tileCount || id < 0) {
        Engine_logError("GfxTileDb_getBitmap() tile %i > tilecount", id);
        Engine_exit(-1);
    }
#endif
    return (struct EngineBitmap*)GfxSprite_getBitmap(gfxSprites[id]);
}

const struct GfxSprite* GfxTileDb_getSprite(int id) {
#ifdef DEBUG
    if (id >= (int)tileCount || id < 0) {
        Engine_logError("GfxTileDb_getSprite() tile %i > tilecount", id);
        Engine_exit(-1);
    }
#endif
    return gfxSprites[id];
}

bool GfxTileDb_advanceFrame(int id, unsigned long time) {
    return GfxSprite_advanceFrame(gfxSprites[id], time);
}

void GfxTileDb_delete(void) {
    if (gfxSprites) {
        for (size_t i = 0; i < tileCount; i++) {
            GfxSprite_delete(gfxSprites[i]);
        }
        GFree(gfxSprites, "GfxTileDb");
        tileCount = 0;
    }
}