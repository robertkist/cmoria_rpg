#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include "engine_bitmap.h"
#include "helpers/h_coordinate.h"
//#include "helpers/h_string.h"

extern void* api;

typedef enum  {
    NO_EVENT,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_A,
    KEY_B,
    KEY_QUIT
} EngineInputEvent;

#ifdef __cplusplus
extern "C" {
#endif

void Engine_exit(int status);
void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y);
void Engine_log(const char *fmt, ...);
void Engine_logError(const char *fmt, ...);
void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap);
void Engine_freeBitmap(void* bitmap);
char* Engine_loadTextFile(const char* fname);
void Engine_drawRect(int x1, int y1, int width, int height); // for debugging
void Engine_clearFrame(void);
unsigned long Engine_getTime(void);
unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2);

#ifdef __cplusplus
}
#endif

#endif