#ifndef GFX_VIEWPORT_H
#define GFX_VIEWPORT_H

#include <stdbool.h>
#include "helpers/h_coordinate.h"
#include "game.h"
#include "engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    STARTUP,
    RUNNING,
    SCROLLING,
} GraphicsState;

void MapDrawing_init(void);
void MapDrawing_delete(void);

GraphicsState getGraphicsState(void);

void updateDrawFirstPass(void);
void updateDrawSecondPass(void);
void updateDrawThirdPass(void);
void calculateMapDrawingVariables(struct HCoordinate pp);
void startScrolling(GameCommand command);
void stopScrolling(void);
bool eventHandlerScrolling(EngineInputEvent event);
bool updateRunning(void);
bool updateScrolling(void);

#ifdef __cplusplus
}
#endif

#endif