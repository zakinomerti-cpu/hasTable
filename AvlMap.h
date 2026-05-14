#ifndef AVLMap_HEADER
#define AVLMap_HEADER

#include <stddef.h>
struct AvlMap;
struct node;

typedef struct key_value_t {
	char* key;
	void* value;
}key_value_t;

typedef struct flatNodeArr {
	key_value_t** keyArr;
	size_t count;
} flatNodeArr;

void flatNodeArr_add(flatNodeArr** a, char* key, void* value);
void flatNodeArr_get_all(flatNodeArr** arr, struct node* p);
void flatNodeArr_delete(flatNodeArr** arr);

typedef struct {
	int (*add)(struct AvlMap*, const void* key,
		size_t key_len, const void* value);
	void* (*get)(struct AvlMap*, const void* key, size_t key_len);
	void** (*get_double_indirect)(struct AvlMap*, const void* key, size_t key_len);
	void (*getAll)(struct AvlMap*,flatNodeArr**);
	int (*remove)(struct AvlMap*, const void* key, size_t key_len);
} AvlMapInterface;

typedef struct AvlMap {
	size_t size;
	struct node* root;
	const AvlMapInterface* ops;
} AvlMap;

AvlMap* AvlMap_new();

#endif