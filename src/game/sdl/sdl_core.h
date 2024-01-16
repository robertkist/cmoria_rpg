#ifndef SDL_CORE_H
#define SDL_CORE_H

#include <stdbool.h>
#include "../engine_bitmap.h"
#include "../engine.h"

int GSDL_showWindow();
void GSDL_close();
void GSDL_drawImage(const struct EngineBitmap* bitmap, int dstx, int dsty);
EngineInputEvent GSDL_getEvent();
bool GSDL_updateFrame();
bool GSDL_isRunning();
struct timeval GSDL_getTime();
void GSDL_stop();
void GSDL_clearFrame();
void GSDL_drawRect(int x1, int y1, int width, int height);

#endif