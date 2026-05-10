#ifndef AVLMap_HEADER
#define AVLMap_HEADER

#include <stddef.h>
struct AvlMap;

typedef struct {
	void (*add)(struct AvlMap*, const void* key, 
		size_t key_len, const void* value);
	void* (*get)(struct AvlMap*, const void* key, size_t key_len);

	void (*set)(struct AvlMap*, const void* key, 
		size_t key_len, const void* value, size_t index);
	void (*prepare)(struct AvlMap*, size_t nsize);
} AvlMapInterface;

typedef struct AvlMap {
	size_t id;
	size_t size;
	size_t capacity;
	const AvlMapInterface* ops;
} AvlMap;

AvlMap AvlMap_create();
AvlMap* AvlMap_new();

#endif