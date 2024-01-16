#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    NO_COMMAND,
    GO_NORTH,
    GO_SOUTH,
    GO_EAST,
    GO_WEST
} GameCommand;

typedef enum  {
    NO_RESULT,
    MOVE_ONE_TILE
} GameCommandResult;

void Game_init(void);
GameCommandResult Game_command(GameCommand command);

#ifdef __cplusplus
}
#endif

#endif