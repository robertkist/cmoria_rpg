#include <string.h>
#include "h_stringarray.h"
#include "h_memory.h"

struct StringArray* StringArray_fromCsv(char* string, int size) {
    struct StringArray* self = (struct StringArray*)GMalloc(sizeof(struct StringArray), "StringArray");
    self->size = size;
    int i = 0;
    int start = 0;
    int len, index;
    self->string = (char**)GMalloc(size * sizeof(char*), "StringArray->string");
    while(i < size) {
        char *e = strchr(string + start, ',');
        if (e == NULL) {
            len = (int)strlen(string) - start;
        } else {
            index = (int)(e - string);
            len = index - start;
        }
        self->string[i] = (char*)GMalloc(len + 1, "StringArray->string[]");
        memcpy(self->string[i], string + start, len);
        i++;
        start += len + 1;
    }
    return self;
}

void StringArray_delete(struct StringArray* self) {
    if (self) {
        for (int i = 0; i < self->size; i++)
            GFree(self->string[i], "StringArray->string[]");
        GFree(self->string, "StringArray->string");
        GFree(self, "StringArray");
    }
}