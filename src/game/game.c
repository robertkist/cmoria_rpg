#include "game.h"
#include "game_player.h"
#include "engine.h"

void Game_init(void) {

}

GameCommandResult Game_command(GameCommand command) {
    GameCommandResult result = NO_RESULT;
    switch(command) {
        case GO_EAST:
            Engine_log("COMMAND: GO_EAST");
            Player_incPosition(1, 0);
            result = MOVE_ONE_TILE;
            break;
        case GO_WEST:
            Engine_log("COMMAND: GO_WEST");
            Player_incPosition(-1, 0);
            result = MOVE_ONE_TILE;
            break;
        case GO_NORTH:
            Engine_log("COMMAND: GO_NORTH");
            Player_incPosition(0, -1);
            result = MOVE_ONE_TILE;
            break;
        case GO_SOUTH:
            Engine_log("COMMAND: GO_SOUTH");
            Player_incPosition(0, 1);
            result = MOVE_ONE_TILE;
            break;
        case NO_COMMAND:
            Engine_log("COMMAND: NO_COMMAND");
            break;
    }
    return result;
}
