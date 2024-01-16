#include "h_hash.h"
#include "h_memory.h"
#include "../engine.h"

/* Constructor */
struct HHash* HHash_new(size_t size) {
    struct HHash* self = (struct HHash*)GMalloc(sizeof(struct HHash), "HHash");
    self->size = size;
    self->items = 0;
    self->hashArray = (struct HHashDataItem**)GMalloc((size_t)size * sizeof(struct HHashDataItem*), "HHash->hashArray");
    return self;
}

/* Creates a hash key for a given string */
unsigned long createHash(char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

void* HHash_search(const struct HHash* self, size_t key) {
    size_t i = key % self->size;  // get the hash
    size_t j = 0; // number of visited elements
    while(j < self->size) {
        if(self->hashArray[i] != NULL) {
            if (self->hashArray[i]->key == key) {
                return self->hashArray[i]->value;
            }
        }
        i++; // go to next cell
        i %= self->size; // wrap around the table
        j++;
    }
    return NULL; // not found
}

void* HHash_strSearch(const struct HHash* self, char* stringKey) {
    size_t key = createHash(stringKey);
    return HHash_search(self, key);
}

bool HHash_insert(struct HHash* self, char* stringKey, const void* value) {
    size_t key = createHash(stringKey);
    if(self->items == self->size) {
        Engine_logError("HHash is full");
        return false; // hash is full - cannot insert new items
    }
    size_t i = key % self->size;
    while(self->hashArray[i] != NULL) {  // move in array until an empty cell found
        if (self->hashArray[i]->key == key) {
            Engine_log("WARNING: HHash key exists");
            return false;
        }
        i++;  // go to next cell
        i %= self->size;  // wrap around the table
    }

    struct HHashDataItem* item = (struct HHashDataItem*)GMalloc(sizeof(struct HHashDataItem), "HHash->hashArray[i]");
    item->value = (void*)value;
    item->key = key;

    self->hashArray[i] = item;
    self->items++;
    return true;
}

/* returns the index of the next entry in hashArray that is not NULL and the entry's key */
size_t next_key(const struct HHash* self, size_t start, size_t *key) {
    for(size_t i=start; i<self->size; i++)
        if(self->hashArray[i]!=NULL) {
            *key = self->hashArray[i]->key;
            return i;
        }
    return self->size;
}

/* Destructor */
void HHash_delete(struct HHash* self) {
    if(self) {
        for(size_t i = 0; i < self->size; i++)
            if(self->hashArray[i] != NULL)
                GFree(self->hashArray[i], "HHash->hashArray[i]");
        GFree(self->hashArray, "HHash->hashArray");
        GFree(self, "HHash");
        self = NULL;
    }
}