#ifndef H_SET_H
#define H_SET_H

#include <stdbool.h>
#include <stddef.h>

struct IntSet {
    size_t size;  // size of the set
    size_t items; // number of items in set
    int* values;
};

struct IntSet* IntSet_new(size_t size);
void IntSet_delete(struct IntSet* self);
void IntSet_clear(struct IntSet* self);
bool IntSet_insert(struct IntSet* self, int value);

#endif