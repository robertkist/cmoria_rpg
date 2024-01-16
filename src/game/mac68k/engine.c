#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include "../engine.h"
#include "../helpers/h_memory.h"
#include "engine_macbitmap.h"

#define MAX_BUF 1000
//#define kTwoPower32 4294967296

struct UnsignedWide {
    unsigned long hi;
    unsigned long lo;
};
typedef struct UnsignedWide UnsignedWide;

pascal void Microseconds(UnsignedWide* m) = {0xA193, 0x225F, 0x22C8, 0x2280}; // requires System 7

unsigned long Engine_getTime(void) {
    // TODO https://68kmla.org/bb/index.php?threads/better-than-tickcount-timing.41563/
    UnsignedWide time;
    Microseconds(&time);
//    unsigned long result = (((double) time.hi) * kTwoPower32) + time.lo;
//    return result;
    return time.lo;
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
    return (time1 - time2) / 1000;
}

void Engine_drawRect(int x1, int y1, int width, int height) {
//    GSDL_drawRect(x1, y1, width, height);
}

void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y) {
    MacBitmap_draw((MacBitmap*)bitmap->bitmap, x, y);
}

void Engine_clearFrame(void) {
//    GSDL_clearFrame();
}

void Engine_log(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
//    printf("LOG: %s\n", buffer1);
    FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "LOG: %s\r", buffer1);
    fclose(fp);
}

void Engine_logError(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
//    fprintf(stderr, "ERROR: %s\n", buffer1);
    FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "ERROR: %s\r", buffer1);
    fclose(fp);
}

//void* Engine_loadBitmap(const char* fname) {
void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    char buffer[MAX_BUF];
    snprintf(buffer, MAX_BUF, ":%s.bmp", fname);

    // TODO move into file loading func
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '/')
            buffer[i] = ':';
    }

    struct MacBitmap* bmp = MacBitmap_new(buffer);
    if (!bmp) {
        Engine_logError("file '%s' does not exist", buffer);
        Engine_exit(-1);
    }

    Engine_log("dimensions: %i %i", bmp->bitmap->bounds.right, bmp->bitmap->bounds.bottom);

    bitmap->bitmap = (void*)bmp;
    bitmap->width = bmp->bitmap->bounds.right;
    bitmap->height = bmp->bitmap->bounds.bottom;
}

void Engine_exit(int status) {
    Engine_logError("*** FATAL ERROR - EXITING ***");
    _exit(status);
}

void Engine_freeBitmap(void* bitmap) {
    MacBitmap_delete((MacBitmap*)bitmap);
}

char* Engine_loadTextFile(const char* fname) {
//    if(access(fname, F_OK) != 0) {
//        Engine_logError("file '%s' does not exist", fname);
//        exit(-1);
//    }
    FILE *fp;
    fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* string = (char*)GMalloc(fsize + 1, "Engine_loadTextFile");
    fread(string, fsize, 1, fp);
    fclose(fp);
    return string;
}