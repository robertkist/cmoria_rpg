#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include "../engine.h"
#include "../helpers/h_memory.h"
#include "sdl_core.h"

static const int MAX_BUF = 1000;

void Engine_drawRect(int x1, int y1, int width, int height) {
    GSDL_drawRect(x1, y1, width, height);
}

void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y) {
    GSDL_drawImage(bitmap, x, y);
}

void Engine_clearFrame(void) {
    GSDL_clearFrame();
}

unsigned long Engine_getTime(void) {
    struct timeval t = GSDL_getTime();
    unsigned long time = (t.tv_sec * 1000000ULL + t.tv_usec) * 1000;
    return time;
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
    return (time1 - time2) / 1000000ULL;
}

void Engine_log(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
    printf("LOG: %s\n", buffer1);
    // write to file log
    FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "LOG: %s\n", buffer1);
    fclose(fp);
}

void Engine_logError(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
    fprintf(stderr, "ERROR: %s\n", buffer1);
    // write to file log
    FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "ERROR: %s\n", buffer1);
    fclose(fp);
}

//void* Engine_loadBitmap(const char* fname) {
void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    char buffer[MAX_BUF];
    snprintf(buffer, MAX_BUF, "%s.bmp", fname);
#ifdef DEBUG
    if(access(buffer, F_OK) != 0) {
        Engine_logError("file '%s' does not exist", buffer);
        exit(-1);
    }
#endif
    SDL_Surface* sdlBitmap = SDL_LoadBMP(buffer);
    bitmap->bitmap = (void*)sdlBitmap;
    bitmap->width = sdlBitmap->w;
    bitmap->height = sdlBitmap->h;
}

void Engine_exit(int status) {
    Engine_logError("*** FATAL ERROR - EXITING ***");
    exit(status);
}

void Engine_freeBitmap(void* bitmap) {
    SDL_FreeSurface((SDL_Surface*)bitmap);
}

void Engine_setPixel(int x, int y, bool white) {
    GSDL_setPixel(x, y, white);
}

char* Engine_loadTextFile(const char* fname) {
#ifdef DEBUG
    if(access(fname, F_OK) != 0) {
        Engine_logError("file '%s' does not exist", fname);
        Engine_exit(-1);
    }
#endif
    FILE *fp;
    fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* string = (char*)GMalloc(fsize + 1, "Engine_loadTextFile");
    size_t bytes = fread(string, fsize, 1, fp);
    if (bytes == 0) {
        Engine_logError("failed to read from file '%s'", fname);
        Engine_exit(-1);
    }
    fclose(fp);
    return string;
}