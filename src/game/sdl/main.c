#include <stdio.h>
#include "../engine.h"
#include "../game_container.h"
#include "../helpers/h_memory.h"
#include "sdl_core.h"

void GSDL_startGameLoop() {
    GameContainer_init();
    do {
        GSDL_updateFrame();
        EngineInputEvent key = GSDL_getEvent();
        if (key == KEY_QUIT) {
            GSDL_stop();
        } else {
            GameContainer_update(key);
        }
    } while(GSDL_isRunning());
    GameContainer_delete();
}

int main(void) {
    GSDL_showWindow();
    GSDL_startGameLoop();
    GSDL_close();
    printf("memory ref count: %li\n", getMemCount());
    return 0;
}