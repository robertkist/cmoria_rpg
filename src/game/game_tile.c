#include <string.h>
#include "game_tile.h"
#include "engine.h"
#include "helpers/h_memory.h"

struct GameTile* GameTile_new(int tileId, const char* tileImage, bool tileBlockMove, bool tileBlockSight) {
    Engine_log("Tile - constructor: tile %s (%i)", tileImage, tileId);
    struct GameTile* self = (struct GameTile*)GMalloc(sizeof(struct GameTile), "GameTile");
    self->id = tileId;
    self->blockMove = tileBlockMove;
    self->blockSight = tileBlockSight;
    self->image = (char*)GMalloc(strlen(tileImage) + 1,"GameTile->image");
    strcpy(self->image, tileImage);
    return self;
}

void GameTile_delete(struct GameTile* self) {
    if (self) {
        Engine_log("Tile - destructor: tile %s (%i)", self->image, self->id);
        if (self->image)
            GFree(self->image, "GameTile->image");
        GFree(self, "GameTile");
    }
}