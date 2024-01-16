#ifndef GAME_TILEDB_H
#define GAME_TILEDB_H

#include <stdbool.h>
#include "engine_bitmap.h"

#ifdef __cplusplus
extern "C" {
#endif

void GfxTileDb_loadTiles(const char* fname);
const struct EngineBitmap* GfxTileDb_getBitmap(int id);
const struct GfxSprite* GfxTileDb_getSprite(int id);
void GfxTileDb_delete(void);
bool GfxTileDb_advanceFrame(int id, unsigned long time);

#ifdef __cplusplus
}
#endif

#endif