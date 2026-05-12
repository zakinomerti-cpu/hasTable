#include <stdio.h>
#include <stdint.h>

#include "hashArray.h"
#include "AvlMap.h"
#include "pVoidArray.h"

static size_t simpleHash(const void* key, size_t key_len) {
	size_t out = 5381;
	uint8_t* bytes = (uint8_t*)key;
	for(size_t iter = 0; iter < key_len; iter+=1) {
		out += bytes[iter]*33;
	}
	return out;
}


static AvlMap* getInnerArray(hashArray* arr, size_t index) {
	AvlMap* tmp = arr->data->ops->get(arr->data, index);
	if (!tmp) {
		tmp = AvlMap_new();
		arr->data->data[index] = tmp; //TODO: pVoidArray.ops->set
		arr->data->size += 1;
	}
	return tmp;
}

static void add(hashArray* arr, const void* key,
	size_t key_len, const void* value) 
{

	if (!key || !value || key_len == 0) return;

	size_t hashIndex = simpleHash(key, key_len) % arr->elementCount;
	AvlMap* innerArr = getInnerArray(arr, hashIndex);

	innerArr->ops->add(innerArr, key, key_len, value);
	size_t size = innerArr->size;
	return;
}

static const void* get(hashArray* arr, const void* key, size_t key_len) {

	if (!key || key_len == 0) return NULL;

	size_t hashIndex = simpleHash(key, key_len) % arr->elementCount;
	AvlMap* innerArr = getInnerArray(arr, hashIndex);

	return innerArr->ops->get(innerArr, key, key_len);
}

static const hashArrayInterface ops = {
	add,
	get,
};

hashArray hashArray_create(size_t size) {
	hashArray arr;
	arr.elementCount = size;
	arr.ops = &ops;
	arr.data = pVoidArray_new();
	arr.data->ops->prepare(arr.data, size);
	return arr;
}
