#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include "h_ini_file.h"
#include "h_memory.h"
#include "../engine.h"

const int MAX_SIZE = 5000;

void csvStrToArray(int* array, char* string, int size) {
    int i = 0;
    int start = 0;
    int len, index;
    char strValue[11];
    while(i < size) {
        char *e = strchr(string + start, ',');
        if (e == NULL) {
            len = (int)strlen(string) - start;
        } else {
            index = (int)(e - string);
            len = index - start;
        }
        memcpy(strValue, string + start, len);
        strValue[len] = '\0';
        array[i] = atoi(strValue);
        i++;
        start += len + 1;
    }
}

struct HIniFile* HIniFile_new(const char* string) {
    struct HIniFile* self = (struct HIniFile*)GMalloc(sizeof(struct HIniFile), "HIniFile");
    self->hash = HHash_new(MAX_SIZE); // NOTE: should be adjusted to actual values
    int insertCounter = 0;
    const char* e;
    size_t index = -1;
    size_t start, len;
    char* section = (char*)GMalloc(8, "HIniFile_new, char* section");
    strcpy(section, "general");
    do {
        // move to next \n
        start = index + 1;
        e = strchr(string + start, '\n');
        if (e == NULL)
            index = strlen(string);
        else
            index = (int) (e - string);
        len = index - start;
        if (string[start] == '[') {  // get section
            const char* m = strchr(string + start, ']');
            if (m != NULL) {
                size_t idx = (int)(m - string);
                if (idx < (start + len)) {
                    free(section);
                    section = (char *) calloc(idx - start - 1 + 1, 1);
                    memcpy(section, string + start + 1, idx - start - 1);
//                        printf("SECTION '%s'\n", section);
                }
            }
        } else if (string[start] != ';') {   // get key=value pair
            const char* m = strchr(string + start, '=');
            if (m != NULL) {
                size_t idx = (int)(m - string);
                if (idx < (start + len)) {
                    size_t i = start;       // key start
                    while (isspace((int)string[i++]));
                    size_t j = idx;         // key end
                    while (isspace((int)string[--j]));
                    size_t k = idx;         // value start index
                    while (isspace((int)string[++k]));
                    size_t l = start + len; // value end index
                    while (isspace((int)string[l--]));
                    if (!(i-1 > j || k > l+1)) {
                        // create compound key: section + ']' + key
                        size_t seclen = strlen(section) + 1;
                        char* key = (char *) calloc(j - i + 2 + seclen + 1, 1);
                        memcpy(key, section, seclen);
                        key[seclen - 1] = ']';
                        memcpy(key + seclen, string + i - 1, j - i + 2);
                        // create value
                        char* value = (char *) calloc(l - k + 2 + 1, 1);
                        memcpy(value, string + k, l - k + 2);
                        // insert int hash
                        HHash_insert(self->hash, key, value);
                        free(key);
                        insertCounter++;
                        if (insertCounter > MAX_SIZE) {
                            Engine_logError("ini has too many entries (max: %i)", MAX_SIZE);
                            Engine_exit(-1);
                        }
                    }
                }
            }
        }
    } while(e != NULL);
    GFree(section, "HIniFile_new, char* section");
    return self;
}

int HIniFile_getInt(struct HIniFile* self, const char* section, const char* key, int defaultValue) {
    char* value = HIniFile_get(self, section, key);
    if (value == NULL) {
        return defaultValue;
    }
    return atoi(value);
}

bool HIniFile_getBool(struct HIniFile* self, const char* section, const char* key, bool defaultValue) {
    char* value = HIniFile_get(self, section, key);
    if (value == NULL) {
        return defaultValue;
    }
    return strcmp(value, "true") == 0;
}

char* HIniFile_get(struct HIniFile* self, const char* section, const char* key) {
    size_t seclen = strlen(section) + 1;
    char* hashKey = (char*)GMalloc(seclen + strlen(key) + 2, "HIniFile_get()");
    memcpy(hashKey, section, seclen);
    hashKey[seclen - 1] = ']';
    memcpy(hashKey + seclen, key, strlen(key));
    char* r = (char*)HHash_strSearch(self->hash, hashKey);
    GFree(hashKey, "HIniFile_get()");
    return r;
}

void HIniFile_delete(struct HIniFile* self) {
    if (self) {
        size_t key = 0;
        HHash_forEachKey(self->hash, key) {
            free((char*)HHash_search(self->hash, key));
        }
        HHash_delete(self->hash);
        GFree(self, "HIniFile");
    }
}
