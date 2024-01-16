#include <stdio.h>
#include <string.h>
#include "game_mapmanager.h"
#include "engine.h"
#include "helpers/h_ini_file.h"
#include "helpers/h_memory.h"
#include "game_map.h"

//static int mapCount = 0;
static int currentMap = 0;
//static struct GameMap** maps = NULL;
struct MapCollection* maps = NULL;

void GameMapManager_loadMaps(struct MapCollection* mapsCollection) {
    maps = mapsCollection;
}

void GameMapManager_setCurrentMap(int current) {
    currentMap = current;
}

int GameMapManager_getTileId(int x, int y, MapLayerType layerType) {
    return GameMap_getTileId(maps->maps[currentMap], x, y, layerType);
}

struct HCoordinate GameMapManager_getSize(void) {
    return GameMap_getSize(maps->maps[currentMap]);
}

void GameMapManager_delete(void) {
    if (maps) {
        for (int i = 0; i < maps->mapCount; i++)
            GameMap_delete(maps->maps[i]);
        GFree(maps->maps, "GameMaps");
        GFree(maps, "MapCollection");
    }
}