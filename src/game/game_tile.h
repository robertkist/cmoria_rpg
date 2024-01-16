#ifndef GAME_TILE_H
#define GAME_TILE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GameTile {
    int id;
    bool blockSight;
    bool blockMove;
    char* image;
};

struct GameTile* GameTile_new(int tileId, const char* tileImage, bool tileBlockMove, bool tileBlockSight);
void GameTile_delete(struct GameTile* self);

#ifdef __cplusplus
}
#endif

#endif