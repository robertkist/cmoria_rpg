#ifndef H_STRINGARRAY_H
#define H_STRINGARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

struct StringArray {
    char** string;
    int size;
};

struct StringArray* StringArray_fromCsv(char* string, int size);
void StringArray_delete(struct StringArray* self);

#ifdef __cplusplus
}
#endif

#endif