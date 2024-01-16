#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../engine.h"
#include "../helpers/h_memory.h"
#include "engine_amigabitmap.h"
#include "engine_amigatime.h"

static const int MAX_BUF = 1000;

void Engine_drawRect(int x1, int y1, int width, int height) {
//    GSDL_drawRect(x1, y1, width, height);
}

void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y) {
    AmigaBitmap_draw(bitmap->bitmap, x, y);
}

void Engine_clearFrame(void) {
//    GSDL_clearFrame();
}

unsigned long Engine_getTime(void) {
    return getTime();
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
//    printf("%li %li\n", time1, time2);
    return (time1 - time2) / 1000;
}

void Engine_log(const char *fmt, ...) {
    // print to buffer
//    char buffer1[4096];
//    va_list args;
//    va_start(args, fmt);
//    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
//    va_end(args);
//    FILE* fp = fopen("log.txt", "a");
//    fprintf(fp, "LOG: %s\n", buffer1);
//    fclose(fp);
}

void Engine_logError(const char *fmt, ...) {
    // print to buffer
//    char buffer1[4096];
//    va_list args;
//    va_start(args, fmt);
//    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
//    va_end(args);
//    FILE* fp = fopen("log.txt", "a");
//    fprintf(fp, "ERROR %s\n", buffer1);
//    fclose(fp);
}

//void* Engine_loadBitmap(const char* fname) {
void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    char buffer[MAX_BUF];
    snprintf(buffer, MAX_BUF, "%s.bmp", fname);
//    if(access(buffer, F_OK) != 0) {
//        Engine_logError("file '%s' does not exist", buffer);
//        exit(-1);
//    }
    struct AmigaBitmap* bmp = AmigaBitmap_load(buffer);
    bitmap->bitmap = (void*)bmp;
    bitmap->width = bmp->width;
    bitmap->height = bmp->height;
}

void Engine_exit(int status) {
    Engine_logError("*** FATAL ERROR - EXITING ***");
    exit(status);
}

void Engine_freeBitmap(void* bitmap) {
    AmigaBitmap_delete((struct AmigaBitmap*)bitmap);
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