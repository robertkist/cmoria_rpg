#ifndef H_INIFILE_H
#define H_INIFILE_H

#include "h_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HIniFile {
    struct HHash* hash;
};

struct HIniFile* HIniFile_new(const char* string);
void HIniFile_delete(struct HIniFile* self);
char* HIniFile_get(struct HIniFile* self, const char* section, const char* key);
int HIniFile_getInt(struct HIniFile* self, const char* section, const char* key, int defaultValue);
bool HIniFile_getBool(struct HIniFile* self, const char* section, const char* key, bool defaultValue);
void csvStrToArray(int* array, char* string, int size);

#ifdef __cplusplus
}
#endif

#endif