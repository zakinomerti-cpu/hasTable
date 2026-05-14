#ifndef hashArray_header
#define hashArray_header

struct pVoidArray;
struct hashArray;

typedef void (*ht_iter_cb)(const void *key, size_t key_len, void *value, void *args);

typedef struct {
	int (*add)(struct hashArray* arr, const void* key,
		size_t key_len, const void* value);
	const void* (*get)(struct hashArray* arr, const void* key, size_t key_len);
	void (*forEach)(struct hashArray* arr, ht_iter_cb cb, void *args);
	void (*resize)(struct hashArray* arr);
	int (*remove)(struct hashArray* arr, const void* key, size_t key_len);
	} hashArrayInterface;

typedef struct hashArray {
	size_t elementCount;
	size_t addedElementCount;
	const hashArrayInterface* ops;
	struct pVoidArray* data;
} hashArray;

hashArray hashArray_create(size_t);
hashArray* hashArray_new(size_t);
void hashArray_delete(hashArray**);

#endif