#include <stdio.h>
#include <unistd.h>
#include <QImage>
#include <QFile>
#include <QDateTime>
#include "../engine.h"
#include "../helpers/h_memory.h"
#include "mainwindow.h"

void Engine_drawBitmap(const struct EngineBitmap* bitmap, int x, int y) {
    MainWindow* w = (MainWindow*)api;
    w->drawImage(bitmap, x, y);
}

unsigned long Engine_getTime(void) {
    return (unsigned long)QDateTime::currentMSecsSinceEpoch();
}

unsigned long Engine_timeDiff(unsigned long time1, unsigned long time2) {
    return time1 - time2;
}

void Engine_clearFrame(void) {
    MainWindow* w = (MainWindow*)api;
    w->clearFrame();
}

void Engine_drawRect(int x1, int y1, int x2, int y2) {
    MainWindow* w = (MainWindow*)api;
    w->drawRect(x1, y1, x2, y2);
}

void Engine_log(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
    printf("LOG: %s\n", buffer1);
}

//void Engine::logError(const String& s) {
//    fprintf(stderr, "ERROR: %s\n", s.string);
//}

void Engine_exit(int status) {
    Engine_logError("*** FATAL ERROR - EXITING ***");
    exit(status);
}

void Engine_logError(const char *fmt, ...) {
    // print to buffer
    char buffer1[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer1, sizeof(buffer1), fmt, args);
    va_end(args);
    fprintf(stderr, "ERROR: %s\n", buffer1);
}

void Engine_loadBitmap(const char* fname, struct EngineBitmap* bitmap) {
    QString qFname = QString(fname) + QString(".png");
    QFile file(qFname);
    if (!file.exists()) {
        fprintf(stderr, "ERROR: bitmap %s not found\n", qFname.toLatin1().data());
        exit(-1);
    }
    QImage* image = new QImage(qFname);
    printf("LOG: loading %s\n", qFname.toLatin1().data());
    if (image == nullptr) {
        fprintf(stderr, "ERROR: bitmap %s not found\n", qFname.toLatin1().data());
        exit(-1);
    }
    bitmap->bitmap = (void*)image;
    bitmap->width = image->width();
    bitmap->height = image->height();
}

void Engine_freeBitmap(void* bitmap) {
    delete (QImage*)bitmap;
}

char* Engine_loadTextFile(const char* fname) {
    if(access(fname, F_OK) != 0) {
        Engine_logError("file %s does not exist", fname);
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