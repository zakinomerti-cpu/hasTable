#include <stdio.h>
#include <stdint.h>

#include "hashArray.h"

#include <stdlib.h>
#include <string.h>

#include "AvlMap.h"
#include "pVoidArray.h"

//toolbox
static size_t simpleHash(const void* key, size_t key_len) {
	size_t out = 5381;
	uint8_t* bytes = (uint8_t*)key;
	for(size_t iter = 0; iter < key_len; iter+=1) {
		out += bytes[iter]*33;
	}
	return out;
}

static size_t rotate_left(size_t value, char n, char r) {
	n %= r;
	return (value << n) | (value >> (r - n));
}

static char to_hex(char b) {
	if(b > 16) return -1;
	if(b < 10) {
		return '0' + b;
	}
	return 'A' + b - 10;
}

static char to_dec(char b) {
	if(b >= 'A' && b <= 'F') {
		return b - 'A' + 10;
	}
	if(b >= '0' && b <= '9') {
		return b - '0';
	}
	return -1;
}

static void* deserialize(const char* key) {
	if(!key) return NULL;
	size_t key_len = strlen(key);
	if (key_len % 2 != 0) return NULL;


	uint8_t* out = malloc(key_len/2);
	if (!out) return NULL;

	uint8_t byte = 0;
	size_t strpos = 0;
	for(size_t i = 0; i < key_len / 2; i+=1) {
		char h = to_dec(key[strpos++]);
		char l = to_dec(key[strpos++]);

		if (h == -1 || l == -1) {
			free(out);
			return NULL;
		}

		byte = (uint8_t)h;
		byte = rotate_left(byte, 4, 8) + (uint8_t)l;
		out[i] = byte;
	}
	return (void*)out;
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

	//free(flatArray.data);

}

static const hashArrayInterface ops = {
	add,
	get,
	forEach
};

hashArray hashArray_create(size_t size) {
	hashArray arr;
	arr.elementCount = size;
	arr.ops = &ops;
	arr.data = pVoidArray_new();
	arr.data->ops->prepare(arr.data, size);
	return arr;
}
