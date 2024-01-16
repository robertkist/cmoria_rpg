#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdbool.h>
#include "helpers/h_coordinate.h"
#include "game_mapmanager.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const int COORD_OUTSIDE_MAP_BOUNDARY;

struct GameMap {
    int id;
    bool wrapAround;
    struct HCoordinate size;
    struct HCoordinate playerStart;
    char* name; // map name (for debug)
    int* floorLayer;
    int* shadowLayer;
    int* wallLayer;
};

struct GameMap* GameMap_new(int _id, bool _wrapARound, const char* _name, struct HCoordinate _size, struct HCoordinate _playerStart, int* floor, int* shadow, int* wall);
void GameMap_delete(struct GameMap* self);
int GameMap_getTileId(const struct GameMap* self, int x, int y, MapLayerType layerType);
struct HCoordinate GameMap_getSize(struct GameMap* self);

#ifdef __cplusplus
}
#endif

#endif