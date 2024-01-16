#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "helpers/h_coordinate.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GamePlayer {
    struct HCoordinate position;
};

void Player_setPosition(int x, int y);
void Player_incPosition(int x, int y);
struct HCoordinate Player_getPosition(void);

#ifdef __cplusplus
}
#endif

#endif