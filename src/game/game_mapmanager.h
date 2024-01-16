#ifndef GAME_MAPMANAGER_H
#define GAME_MAPMANAGER_H

#include "helpers/h_coordinate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    FLOOR_LAYER,
    SHADOW_LAYER,
    WALL_LAYER,
} MapLayerType;

struct MapCollection {
    int mapCount;
    struct GameMap** maps;
};

void GameMapManager_loadMaps(struct MapCollection* maps);
void GameMapManager_delete(void);
void GameMapManager_setCurrentMap(int current);
int GameMapManager_getTileId(int x, int y, MapLayerType layerType);
struct HCoordinate GameMapManager_getSize(void);

#ifdef __cplusplus
}
#endif

#endif