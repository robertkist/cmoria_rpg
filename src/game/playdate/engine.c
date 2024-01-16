#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "pd_api.h"
#include "../engine.h"
#include "../helpers/h_memory.h"

void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->graphics->drawBitmap((LCDBitmap*)EngineBitmap_getBitmap(bitmap), x, y, kBitmapUnflipped);
}

unsigned long Engine_getTime(void) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    return (unsigned long)pd->system->getCurrentTimeMilliseconds();
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
    return time1 - time2;
}

void Engine_clearFrame(void) {
//    PlaydateAPI* pd = (PlaydateAPI*)api;
//    pd->graphics->clear(kColorWhite);
}

void Engine_drawRect(int x1, int y1, int width, int height) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->graphics->drawRect(x1, y1, width, height, kColorBlack);
}

void Engine_exit(int status) {
    Engine_logError("*** FATAL ERROR - PLEASE EXIT & RESTART! ***");
}

void Engine_log(const char *fmt, ...) {
    // Variadic parameters crash the playdate
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->system->logToConsole(fmt);
}

void Engine_logError(const char *fmt, ...) {
    // Variadic parameters crash the playdate
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->system->logToConsole(fmt);
}

void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    LCDBitmap* lcdBitmap = pd->graphics->loadBitmap(fname, NULL);
    if (lcdBitmap == NULL) {
        pd->system->logToConsole("failed to load bitmap");
//        Engine_logError("failed to load bitmap '%s", fname); // TODO
        Engine_exit(-1);
    }
    bitmap->bitmap = (void*)lcdBitmap;
    pd->graphics->getBitmapData((LCDBitmap*)lcdBitmap, &bitmap->width, &bitmap->height, NULL, NULL, NULL);
}

void Engine_freeBitmap(void* bitmap) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    pd->graphics->freeBitmap((LCDBitmap*)bitmap);
}

char* Engine_loadTextFile(const char* fname) {
    PlaydateAPI* pd = (PlaydateAPI*)api;
    SDFile* file = pd->file->open(fname, kFileRead);
    if (file == NULL) {
        pd->system->logToConsole("ERROR: cannot load file");
//        Engine_log("ERROR: cannot load file"); // TODO
        Engine_exit(-1);
    }
    pd->file->seek(file, 0, SEEK_END);
    size_t fsize = pd->file->tell(file);
    pd->file->seek(file, 0, SEEK_SET);
    char* string = (char*)GMalloc(fsize + 1, "Engine_loadTextFile");
    pd->file->read(file, string, fsize);
    pd->file->close(file);
    return string;
}
