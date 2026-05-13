#include <stdio.h>
#include <stdint.h>

#include "hashArray.h"

#include <stdlib.h>
#include <string.h>

#include "AvlMap.h"
#include "pVoidArray.h"

#include "serialize_to_bytes.h"

//toolbox
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
		arr->data->ops->set(arr->data, tmp, index);
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
	arr->addedElementCount += 1;

	if (arr->addedElementCount % 20 == 0) {
		double load_factor = (double)arr->addedElementCount / (double)arr->elementCount;
		if (load_factor > 0.7) {
			//resize(arr);
		}
	}
}

static const void* get(hashArray* arr, const void* key, size_t key_len) {

	if (!key || key_len == 0) return NULL;

	size_t hashIndex = simpleHash(key, key_len) % arr->elementCount;
	AvlMap* innerArr = getInnerArray(arr, hashIndex);

	return innerArr->ops->get(innerArr, key, key_len);
}

static void forEach(hashArray* arr, ht_iter_cb cb, void *args) {
	if (!arr || !cb) return;

	flatNodeArr* flatArray = NULL;
	for (size_t iter = 0; iter < arr->elementCount; iter+=1) {
		AvlMap* tmp = arr->data->ops->get(arr->data, iter);
		if (!tmp) continue;
		flatNodeArr_get_all(&flatArray, tmp->root);

		for (size_t j = 0; j < flatArray->count; j+=1) {
			void* value = flatArray->keyArr[j]->value;
			char* string_key = flatArray->keyArr[j]->key;
			size_t key_len = strlen(string_key) / 2;

			void* key = deserialize(string_key);

			cb(key, key_len, value, args);
			free(key);
		}

		flatNodeArr_delete(&flatArray);
	}

}

static void resize(hashArray* arr) {
	if (!arr) return;

	flatNodeArr* flatArray = NULL;
	size_t new_size = arr->elementCount*2;
	pVoidArray* new_array = pVoidArray_new();
	new_array->ops->prepare(new_array, new_size);


	for (size_t iter = 0; iter < arr->elementCount; iter+=1) {
		AvlMap* tmp = arr->data->ops->get(arr->data, iter);
		if (!tmp) continue;
		flatNodeArr_get_all(&flatArray, tmp->root);

		for (size_t j = 0; j < flatArray->count; j+=1) {
			char* string_key = flatArray->keyArr[j]->key;
			void* value = flatArray->keyArr[j]->value;

			size_t key_len = strlen(string_key) / 2;
			void* key = deserialize(string_key);

			size_t hashIndex = simpleHash(key, key_len) % new_size;
			AvlMap* node = new_array->ops->get(new_array, hashIndex);
			if (!node) {
				node = AvlMap_new();
				new_array->ops->set(new_array, node, hashIndex);
			}

			node->ops->add(node, key, key_len, value);
			free(key);
		}

		flatNodeArr_delete(&flatArray);
	}
	free(arr->data); arr->data = new_array;
	arr->elementCount = new_size;
}

static const hashArrayInterface ops = {
	add,
	get,
	forEach,
	resize
};

hashArray hashArray_create(size_t size) {
	hashArray arr;
	arr.elementCount = size;
	arr.addedElementCount = 0;
	arr.ops = &ops;
	arr.data = pVoidArray_new();
	arr.data->ops->prepare(arr.data, size);
	return arr;
}
