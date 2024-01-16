#include <stdio.h>
#include "game_tilemanager.h"
#include "engine.h"
#include "helpers/h_ini_file.h"
#include "helpers/h_memory.h"
#include "game_tile.h"

static struct GameTile** tiles = NULL;
static size_t tileCount = 0;

void GameTileManager_init(void) {
    Engine_log("GameTileManager_init");
}

void GameTileManager_delete(void) {
    if (tiles) {
        for (size_t i = 0; i < tileCount; i++) {
            GameTile_delete(tiles[i]);
        }
        GFree(tiles, "GameTileManager, GameTile** tiles");
        tileCount = 0;
        Engine_log("TileManager_delete");
    }
}

void GameTileManager_loadTiles(const char* fname) {
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
    tiles = (struct GameTile**)GMalloc(tileCount * sizeof(struct GameTile*), "GameTileManager, GameTile** tiles");
    for (size_t i = 0; i < tileCount; i++) {
        char section[40];
        snprintf(section, 40, "%li", (long int)i);
        char* image = HIniFile_get(ini, section, "image");
        bool blockSight = HIniFile_getBool(ini, section, "block_sight", false);
        bool blockMove = HIniFile_getBool(ini, section, "block_move", false);
        tiles[i] = GameTile_new((int)i, image, blockMove, blockSight);
    }
    HIniFile_delete(ini);
}

size_t GameTileManager_getTileCount(void) {
    return tileCount;
}

const struct GameTile* GameTileManager_getTile(int id) {
#ifdef DEBUG
    if (id >= (int)tileCount || id < 0) {
        Engine_logError("GameTileManager::getTile() tile %i > tilecount", id);
        Engine_exit(-1);
    }
#endif
    return tiles[id];
}
