#include "pVoidArray.h"
#include <stdlib.h>
#include <string.h>

static void add(pVoidArray* arr, void* element) {
	if(arr->size >= arr->capacity) {
		arr->capacity = (arr->capacity == 0) ? 2 :
			arr->capacity * 2;
		void** tmp = realloc(arr->data, arr->capacity * sizeof(void*));
		if(!tmp) return;
		arr->data = tmp;
	}
	
	arr->data[arr->size] = element;
	arr->size += 1;
}

static void set(pVoidArray* arr, void* data, size_t index) {
	if(index >= arr->capacity) return;
	arr->data[index] = data;
}

static void* get(const pVoidArray* arr, size_t index) {
	if(index >= arr->capacity) return NULL;
	return arr->data[index];
}

static void* last(const pVoidArray* arr) {
	return arr->data[arr->size-1];
}

static size_t size(const pVoidArray* arr) {
	return arr->size;
}

static void prepare(pVoidArray* self, size_t new_size) {
	if(!self) return;

	size_t new_capacity = self->size + new_size;
	void** tmp = realloc(self->data, sizeof(void*)*new_capacity);
	if(!tmp) return; self->data = tmp;

	memset(self->data + self->size, 0,(new_capacity - self->size) * sizeof(void*));
	self->capacity = new_capacity;
}

static const pVoidArrayInterface ops = {
	get,
	last,
	set,
	add,
	size,
	prepare
};

pVoidArray pVoidArray_create() {
	pVoidArray arr = {0};
	arr.size = 0;
	arr.capacity = 0;
	arr.data = NULL;
	arr.ops = &ops;
	return arr;
}

pVoidArray* pVoidArray_new() {
	pVoidArray* arr = malloc(sizeof(pVoidArray));
	arr->size = 0;
	arr->capacity = 0;
	arr->data = NULL;
	arr->ops = &ops;
	return arr;
}

void pVoidArray_delete(pVoidArray** arr) {
	free((*arr)->data);
	(*arr) = NULL;
}