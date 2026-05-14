#include "hastable_iface.h"
#include "../hashArray.h"
#include <stdlib.h>

typedef struct ht {
    hashArray* arr;
} ht_t;

ht_t* ht_create(size_t initial_capacity) {
    ht_t* arr = malloc(sizeof(ht_t));
    arr->arr = hashArray_new(initial_capacity);
    return arr;
}

void ht_destroy(ht_t *table) { // незакончена!!!
    free(table->arr);
    free(table);
}

int ht_put(ht_t *table, const void *key, size_t key_len, const void *value) {
    return table->arr->ops->add(table->arr, key, key_len, value);
}

void *ht_get(const ht_t *table, const void *key, size_t key_len) {
    return (void* const)table->arr->ops->get(table->arr, key, key_len);
}

int ht_remove(ht_t *table, const void *key, size_t key_len) {
    return table->arr->ops->remove(table->arr, key, key_len);
}

size_t ht_size(const ht_t *table) {
    return table->arr->addedElementCount;
}

double ht_load_factor(const ht_t *table) {
    return (double)table->arr->addedElementCount / (double)table->arr->elementCount;
}

void ht_foreach(ht_t *table, ht_iter_cb cb, void *args) {
    table->arr->ops->forEach(table->arr, cb, args);
}

