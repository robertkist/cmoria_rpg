#ifndef GAME_TILEMANAGER_H
#define GAME_TILEMANAGER_H

#include "game_tile.h"

#ifdef __cplusplus
extern "C" {
#endif

void GameTileManager_init(void);
void GameTileManager_delete(void);
void GameTileManager_loadTiles(const char* fname);
size_t GameTileManager_getTileCount(void);
const struct GameTile* GameTileManager_getTile(int id);

#ifdef __cplusplus
}
#endif

#endif