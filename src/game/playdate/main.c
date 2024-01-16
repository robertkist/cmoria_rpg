#include "pd_api.h"
#include "../engine.h"
#include "../game_container.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

void* api;
static int update(void* userdata);

EngineInputEvent getPDInputEvent(void) {
    PDButtons pressed;
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->system->getButtonState(NULL, &pressed, NULL);
    if (pressed & kButtonUp) {
        pd->system->logToConsole("up");
        return KEY_UP;
    } else if (pressed & kButtonDown) {
        pd->system->logToConsole("down");
        return KEY_DOWN;
    } else if (pressed & kButtonLeft) {
        pd->system->logToConsole("left");
        return KEY_LEFT;
    } else if (pressed & kButtonRight) {
        pd->system->logToConsole("right");
        return KEY_RIGHT;
    } else if (pressed & kButtonA) {
        pd->system->logToConsole("A");
        return KEY_A;
    } else if (pressed & kButtonB) {
        pd->system->logToConsole("B");
        return KEY_B;
    }
    return NO_EVENT;
}


int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
    (void) arg; // arg is currently only used for event = kEventKeyPressed
    api = pd;
    if (event == kEventInit) {
        pd->system->logToConsole("running...");
        GameContainer_init();
        pd->system->setUpdateCallback(update, pd);
    }
    if (event == kEventTerminate) {
        GameContainer_delete();
    }
    return 0;
}

static int update(void* userdata) {
    api = (PlaydateAPI*)userdata;
    GameContainer_update(getPDInputEvent());
    ((PlaydateAPI*)api)->system->drawFPS(0,0);
    return 1;
}