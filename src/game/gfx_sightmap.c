#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "gfx_sightmap.h"
#include "helpers/h_memory.h"
#include "config.h"
#include "game_mapmanager.h"
#include "game_tilemanager.h"

#define setSightMapVisible(x, y) sightMap[((y) * sightMapSize.width) + (x)] = TILE_VISIBLE
#define setSightMapInvisible(x, y) sightMap[((y) * sightMapSize.width) + (x)] = TILE_INVISIBLE
#define getSightMapD(x, y) sightMap[((y) * sightMapSize.width) + (x)]

#define TILE_VISIBLE 1
#define TILE_INVISIBLE 0


static int8_t * sightMap = NULL;
static int8_t halfW, halfH;
static const int mapBorder = 2;
struct HCoordinate sightMapSize;
struct HCoordinate sightMapHalfSize;

void clearSightMap(void) {
    for (size_t i = 0; i < (size_t)(sightMapSize.width * sightMapSize.height); i++)
        sightMap[i] = TILE_INVISIBLE;
}

void SightMap_init(void) {
    sightMapSize.width = viewportSizeT.width + mapBorder;
    sightMapSize.height = viewportSizeT.height + mapBorder;
    sightMap = (int8_t*)GMalloc(sightMapSize.width * sightMapSize.height * sizeof(int8_t), "SightMap");
    clearSightMap();
    halfW = (int8_t )(sightMapSize.width - 1) / 2;
    halfH = (int8_t )(sightMapSize.height - 1) / 2;
    sightMapHalfSize.width = halfW; // TODO redundant info with HalfH and HalfW
    sightMapHalfSize.height = halfH;
}

void SightMap_delete(void) {
    GFree(sightMap, "SightMap");
}

int getSightMap(int x, int y) {
    return sightMap[y * sightMapSize.width + x];
}

bool tileBlocksSight(int x, int y) {
    // returns if a given tile blocks the sight or not
    int tileId = GameMapManager_getTileId(x, y, WALL_LAYER);
    if (tileId > 0)  // not 0 and not -1
        return GameTileManager_getTile(tileId)->blockSight;
    return false;
}

void calculateSight(struct HCoordinate pp) {
    clearSightMap();
    struct HCoordinate map = { .x=pp.x + -halfW, .y=pp.y + -halfH };  // TODO -/+
    // 9x9 area around the player is always visible
    setSightMapVisible(halfW - 1, halfH - 1);
    setSightMapVisible(halfW, halfH - 1);
    setSightMapVisible(halfW + 1, halfH - 1);
    setSightMapVisible(halfW - 1, halfH);
    setSightMapVisible(halfW, halfH);
    setSightMapVisible(halfW + 1, halfH);
    setSightMapVisible(halfW - 1, halfH + 1);
    setSightMapVisible(halfW, halfH + 1);
    setSightMapVisible(halfW + 1, halfH + 1);
    // cardinal directions
    // TODO CRASH!!!!
    for (int x = (halfW - 1); x >= 0; x--)  // left of player
        if (getSightMapD(x + 1, halfH) == 1 && !tileBlocksSight(map.x + x + 1, map.y + halfH))
            setSightMapVisible(x, halfH);
    for (int y = (halfH - 1); y >= 0; y--) // up
        if (getSightMapD(halfW, y + 1) == 1 && !tileBlocksSight(map.x + halfW, map.y + y + 1))
            setSightMapVisible(halfW, y);
    for (int x = (halfW + 1); x < sightMapSize.width; x++) // right of player
        if (getSightMapD(x - 1, halfH) == 1 && !tileBlocksSight(map.x + x - 1, map.y + halfH))
            setSightMapVisible(x, halfH);
    for (int y = (halfH + 1); y < sightMapSize.height; y++) // down
        if (getSightMapD(halfW, y - 1) == 1 && !tileBlocksSight(map.x + halfW, map.y + y - 1))
            setSightMapVisible(halfW, y);
    // quadrants
    for (int y = (halfH + 1); y < sightMapSize.height; y++) {
        for (int x = (halfW - 1); x >= 0; x--) {
            if (getSightMapD(x, y - 1) == 1 && !tileBlocksSight(map.x + x, map.y + y - 1))
                setSightMapVisible(x, y);
            else if (getSightMapD(x + 1, y) == 1 && !tileBlocksSight(map.x + x + 1, map.y + y))
                setSightMapVisible(x, y);
            else if (getSightMapD(x + 1, y - 1) == 1 && !tileBlocksSight(map.x + x + 1, map.y + y - 1))
                setSightMapVisible(x, y);
            if (tileBlocksSight(map.x + x, map.y + y - 1) && tileBlocksSight(map.x + x + 1, map.y + y) && !tileBlocksSight(map.x + x, map.y + y))
                setSightMapInvisible(x, y);
        }
        for (int x = (halfW + 1); x < sightMapSize.width; x++) {
            if (getSightMapD(x, y - 1) == 1 && !tileBlocksSight(map.x + x, map.y + y - 1))
                setSightMapVisible(x, y);
            else if (getSightMapD(x - 1, y) == 1 && !tileBlocksSight(map.x + x - 1, map.y + y))
                setSightMapVisible(x, y);
            else if (getSightMapD(x - 1, y - 1) == 1 && !tileBlocksSight(map.x + x - 1, map.y + y - 1))
                setSightMapVisible(x, y);
            if (tileBlocksSight(map.x + x, map.y + y - 1) && tileBlocksSight(map.x + x - 1, map.y + y) && !tileBlocksSight(map.x + x, map.y + y))
                setSightMapInvisible(x, y);
        }
    }
    for (int y = (halfH - 1); y >= 0; y--) {
        for (int x = (halfW - 1); x >= 0; x--) {
            if (getSightMapD(x, y + 1) == 1 && !tileBlocksSight(map.x + x, map.y + y + 1))
                setSightMapVisible(x, y);
            else if (getSightMapD(x + 1, y) == 1 && !tileBlocksSight(map.x + x + 1, map.y + y))
                setSightMapVisible(x, y);
            else if (getSightMapD(x + 1, y + 1) == 1 && !tileBlocksSight(map.x + x + 1, map.y + y + 1))
                setSightMapVisible(x, y);
            if (tileBlocksSight(map.x + x, map.y + y + 1) && tileBlocksSight(map.x + x + 1, map.y + y) && !tileBlocksSight(map.x + x, map.y + y))
                setSightMapInvisible(x, y);
        }
        for (int x = (halfW + 1); x < sightMapSize.width; x++) {
            if (getSightMapD(x, y + 1) == 1 && !tileBlocksSight(map.x + x, map.y + y + 1))
                setSightMapVisible(x, y);
            else if (getSightMapD(x - 1, y) == 1 && !tileBlocksSight(map.x + x - 1, map.y + y))
                setSightMapVisible(x, y);
            else if (getSightMapD(x - 1, y + 1) == 1 && !tileBlocksSight(map.x + x - 1, map.y + y + 1))
                setSightMapVisible(x, y);
            if (tileBlocksSight(map.x + x, map.y + y + 1) && tileBlocksSight(map.x + x - 1, map.y + y) && !tileBlocksSight(map.x + x, map.y + y))
                setSightMapInvisible(x, y);
        }
    }
    // 4 cardinal directions
    for (int x = (halfW + 1); x < sightMapSize.width; x++) { // right
        if ((getSightMapD(x, halfH - 1) == TILE_VISIBLE || getSightMapD(x, halfH + 1) == TILE_VISIBLE))
            setSightMapVisible(x, halfH);
        if (tileBlocksSight(map.x + x, map.y + halfH) && !tileBlocksSight(map.x + x + 1, map.y + halfH))
            break;
    }
    for (int x = (halfW - 1); x >= 0; x--) { // left
        if ((getSightMapD(x, halfH - 1) == TILE_VISIBLE || getSightMapD(x, halfH + 1) == TILE_VISIBLE))
            setSightMapVisible(x, halfH);
        if (tileBlocksSight(map.x + x, map.y + halfH) && !tileBlocksSight(map.x + x - 1, map.y + halfH))
            break;
    }
    for (int y = (halfH + 1); y < sightMapSize.height; y++) { // down
        if ((getSightMapD(halfW - 1, y) == TILE_VISIBLE || getSightMapD(halfW + 1, y) == TILE_VISIBLE))
            setSightMapVisible(halfW, y);
        if (tileBlocksSight(map.x + halfW, map.y + y) && !tileBlocksSight(map.x + halfW, map.y + y + 1))
            break;
    }
    for (int y = (halfH - 1); y >= 0; y--) { // up
        if ((getSightMapD(halfW - 1, y) == TILE_VISIBLE || getSightMapD(halfW + 1, y) == TILE_VISIBLE))
            setSightMapVisible(halfW, y);
        if (tileBlocksSight(map.x + halfW, map.y + y) && !tileBlocksSight(map.x + halfW, map.y + y - 1))
            break;
    }
}