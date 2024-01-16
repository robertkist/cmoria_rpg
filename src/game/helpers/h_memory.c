#include <stdlib.h>
#include <stdio.h>
#include "h_memory.h"

// ATTENTION PLAYDATE: DEBUG MUST BE UNDEFINED or else: compile/link errors
//#define DEBUG 1

size_t memCount = 0;

void GFree(void* obj, const char* str) {
    free(obj);
    memCount--;
#ifdef DEBUG
    printf("GFREE: %s, %li\n", str, memCount);
    FILE* fp = fopen("memory_log.txt", "a");
    fprintf(fp, "GFREE: %s, %li\n", str, memCount);
    fclose(fp);
#endif
}

void* GMalloc(size_t size, const char* str) {
    memCount++;
#ifdef DEBUG
    printf("GMALLOC: %s (%li)\n", str, memCount);
    FILE* fp = fopen("memory_log.txt", "a");
    fprintf(fp, "GMALLOC: %s, %li\n", str, memCount);
    fclose(fp);
#endif
    return calloc(1, size);
//    return malloc(size); // TODO malloc is faster but causes errors somewhere (HInifile?)
}

size_t getMemCount(void) {
    return memCount;
}