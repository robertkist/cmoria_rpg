#ifndef H_MEMORY_H
#define H_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void GFree(void* obj, const char* str);
void* GMalloc(size_t size, const char* str);
size_t getMemCount(void);

#ifdef __cplusplus
}
#endif

#endif