#ifndef hashArray_header
#define hashArray_header

#include <stddef.h>

struct pVoidArray;
struct hashArray;

typedef struct {
	void (*add)(struct hashArray* arr, const void* key,
		size_t key_len, const void* value);
	void* (*get)(struct hashArray* arr, const void* key, size_t key_len);
	} hashArrayInterface;

typedef struct hashArray {
	size_t elementCount;
	const hashArrayInterface* ops;
	struct pVoidArray* data;
} hashArray;

hashArray hashArray_create(int);

#endif