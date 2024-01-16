#include "h_set.h"
#include "h_memory.h"
#include "../engine.h"

#include <stdio.h>

struct IntSet* IntSet_new(size_t size) {
    struct IntSet* self = (struct IntSet*)GMalloc(sizeof(struct IntSet), "IntSet");
    self->size = size;
    self->items = 0;
    self->values = (int*)GMalloc(size * sizeof(int), "IntSet->values");
    for (size_t i = 0; i < size; i++) {
        self->values[i] = -1;
    }
    return self;
}

void IntSet_delete(struct IntSet* self) {
    if(self) {
        GFree(self->values, "IntSet->values");
        GFree(self, "IntSet");
        self = NULL;
    }
}

void IntSet_clear(struct IntSet* self) {
    size_t items = 0;
    for (size_t i = 0; i < self->size && items < self->items; i++) {
        if (self->values[i] != -1)
            items++;
        self->values[i] = -1;
    }
    self->items = 0;
}

bool IntSet_insert(struct IntSet* self, int value) {
    size_t key = value;
    if(self->items == self->size) {
        Engine_logError("IntSet is full!\n");
        return false; // hash is full - cannot insert new items
    }
    size_t i = key % self->size;
    while(self->values[i] != -1) {  // move in array until an empty cell found
        if (self->values[i] == value) {
            return true; // value exists
        }
        i++;  // go to next cell
        i %= self->size;  // wrap around the table
    }
//    printf("--INSERT %zu %i %zu\n", i, value, self->items);
    self->values[i] = value;
    self->items++;
    return true;
}