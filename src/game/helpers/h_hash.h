#ifndef H_HASH_H
#define H_HASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct HHashDataItem {
    size_t key;
    void* value;
};

struct HHash {
    size_t size; // size of the hash
    size_t items; // number of items in hash
    struct HHashDataItem **hashArray;
};

size_t next_key(const struct HHash* self, size_t start, size_t *key);

struct HHash* HHash_new(size_t size);
void HHash_delete(struct HHash* self);
void* HHash_strSearch(const struct HHash* self, char* stringKey);
void* HHash_search(const struct HHash* self, size_t key);
bool HHash_insert(struct HHash* self, char* stringKey, const void* value);
#define HHash_forEachKey(hash, key) key=0;for(size_t i=next_key(hash, 0, &(key)); i<(hash)->size; i=next_key(hash, (size_t)(i+1), &(key)))

#endif  // H_HASH_H
