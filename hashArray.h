#ifndef hashArray_header
#define hashArray_header

struct pVoidArray;
struct hashArray;

typedef struct {
	void (*add)(struct hashArray* arr, const void* key,
		size_t key_len, const void* value);
	const void* (*get)(struct hashArray* arr, const void* key, size_t key_len);
	} hashArrayInterface;

typedef struct hashArray {
	size_t elementCount;
	size_t addedElementCount;
	const hashArrayInterface* ops;
	struct pVoidArray* data;
} hashArray;

hashArray hashArray_create(size_t);

#endif