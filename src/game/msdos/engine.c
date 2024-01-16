#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "engine_bitmap_msdos.h"
#include "../engine.h"
#include "vesa_api.h"
#include "../helpers/h_memory.h"

void Engine_drawBitmap(const EngineBitmap* bitmap, int x, int y) {
    VBE_draw_bitmap((GfxBitmap*)EngineBitmap_getBitmap(bitmap), x, y);
}

void Engine_clearFrame(void) {
    VBE_clear();
}

void Engine_drawRect(int x1, int y1, int width, int height) {
    VBE_draw_rect(x1, y1, width, height);
}

void Engine_exit(int status) {
    exit(status);
}

unsigned long Engine_getTime(void) {
    return (unsigned long)clock();
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
    return (time1 - time2) * 10;
}

void Engine_log(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
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
    FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "ERROR %s\n", buffer1);
    fclose(fp);
}

//String msdosFileNameConverter(const char* str) {
char* msdosFileNameConverter(const char* str) {
    const char* c = str; // pointer to first character
    size_t len = strlen(str);
    unsigned long v = 0;
    for (size_t i = 0; i < len; i++) {
        v += (*c * *c * (i + 1));
        c++;
    }
    v += len;
    char* out = (char*)GMalloc(40, "msdosFileNameConverter");
    sprintf(out, "tiles/%lu", v);
    return out;
}

void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    char* fh = msdosFileNameConverter(fname);
    GfxBitmap* dosBitmap = GfxBitmap_new(fh);
    if (dosBitmap == NULL) {
        Engine_logError("loading bitmap '%s (aka %s)'", fname, fh);
        exit(-1);
    }
    GFree(fh, "msdosFileNameConverter");
    bitmap->bitmap = (void*)dosBitmap;
    bitmap->width = dosBitmap->width;
    bitmap->height = dosBitmap->height;
}

void Engine_freeBitmap(void* bitmap) {
    GfxBitmap_delete((GfxBitmap*)bitmap);
}

char* Engine_loadTextFile(const char* fname) {
    if(access(fname, F_OK) != 0) {
        Engine_logError("file '%s' does not exist", fname);
        exit(-1);
    }
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
