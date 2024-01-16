#include <stdlib.h>
#include <string.h>
#include "game_map.h"
#include "engine.h"
#include "helpers/h_memory.h"

const int COORD_OUTSIDE_MAP_BOUNDARY = -1;

struct GameMap* GameMap_new(int _id,
        bool _wrapARound,
        const char* _name,
        struct HCoordinate _size,
        struct HCoordinate _playerStart,
        int* floor, int* shadow, int* wall) {
    struct GameMap* self = (struct GameMap*)GMalloc(sizeof(struct GameMap), "GameMap");
    self->id = _id;
    self->wrapAround = _wrapARound;
    self->name = (char*)GMalloc(strlen(_name) + 1, "GameMap_name");
    strcpy(self->name, _name);
    self->size = _size;
    self->playerStart = _playerStart;
    size_t layerSize = sizeof(int) * _size.x * _size.y;

    self->floorLayer = (int*)GMalloc(layerSize, "GameMap_floorLayer");
    memcpy(self->floorLayer, floor, layerSize);

    self->shadowLayer = (int*)GMalloc(layerSize, "GameMap_shadowLayer");
    memcpy(self->shadowLayer, shadow, layerSize);

    self->wallLayer = (int*)GMalloc(layerSize, "GameMap_wallLayer");
    memcpy(self->wallLayer, wall, layerSize);
    return self;
}

void GameMap_delete(struct GameMap* self) {
    if (self) {
        GFree(self->name, "GameMap_name");
        GFree(self->floorLayer, "GameMap_floorLayer");
        GFree(self->shadowLayer, "GameMap_shadowLayer");
        GFree(self->wallLayer, "GameMap_wallLayer");
        GFree(self, "GameMap");
    }
}

struct HCoordinate GameMap_getSize(struct GameMap* self) {
    return self->size;
}

int GameMap_getTileId(const struct GameMap* self, int x, int y, MapLayerType layerType) {
    if (self->wrapAround) {
        x %= self->size.x;
        if (x < 0)
            x += self->size.x;
        y %= self->size.y;
        if (y < 0)
            y += self->size.y;
    } else {
        if (x < 0 || x >= self->size.x || y < 0 || y >= self->size.y)
            return COORD_OUTSIDE_MAP_BOUNDARY; // coordinates outside map bounds
    }
    switch(layerType) {
        case FLOOR_LAYER:
            return self->floorLayer[y * self->size.x + x];
        case SHADOW_LAYER:
            return self->shadowLayer[y * self->size.x + x];
        case WALL_LAYER:
            return self->wallLayer[y * self->size.x + x];
    }
    Engine_logError("GameMap_getTileId() - tile (%i, %i) not found", x, y);
    Engine_exit(-1);
    return -1;
}

