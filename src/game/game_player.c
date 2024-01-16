#include "game_player.h"
#include "game_mapmanager.h"

static struct GamePlayer player;

void Player_setPosition(int x, int y) {
    struct HCoordinate mapDimension = GameMapManager_getSize();
    player.position.x = x % mapDimension.width;
    player.position.y = y % mapDimension.height;
    if (player.position.x < 0)
        player.position.x += mapDimension.width;
    if (player.position.y < 0)
        player.position.y += mapDimension.height;
}

void Player_incPosition(int x, int y) {
    struct HCoordinate mapDimension = GameMapManager_getSize();
    player.position.x = (player.position.x + x) % mapDimension.width;
    player.position.y = (player.position.y + y) % mapDimension.height;
    if (player.position.x < 0)
        player.position.x += mapDimension.width;
    if (player.position.y < 0)
        player.position.y += mapDimension.height;
}

struct HCoordinate Player_getPosition(void) {
    struct HCoordinate coordinate = {
            .x=player.position.x,
            .y=player.position.y};
    return coordinate;
}