#ifndef GAME_CONTAINER_H
#define GAME_CONTAINER_H

#include <stdbool.h>
#include "../engine.h"

#ifdef __cplusplus
extern "C" {
#endif

void GameContainer_init(void);
bool GameContainer_update(EngineInputEvent event);
void GameContainer_handleInput(EngineInputEvent event);
void GameContainer_delete(void);

#ifdef __cplusplus
}
#endif

#endif
