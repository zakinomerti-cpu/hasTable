#ifndef AVLMap_HEADER
#define AVLMap_HEADER

#include <stddef.h>
struct AvlMap;
struct node;

typedef struct {
	void (*add)(struct AvlMap*, const void* key, 
		size_t key_len, const void* value);
	void* (*get)(struct AvlMap*, const void* key, size_t key_len);
} AvlMapInterface;

typedef struct AvlMap {
	size_t size;
	struct node* root;
	const AvlMapInterface* ops;
} AvlMap;

AvlMap* AvlMap_new();

#endif