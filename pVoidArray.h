#ifndef pVoidArray_header
#define pVoidArray_header


#include <stddef.h>
struct pVoidArray;

typedef struct {
	//getters
	void* (*get)(const struct pVoidArray*, size_t index);
	void* (*last)(const struct pVoidArray*);

	//setters
	void (*set)(struct pVoidArray*, void* data, size_t index);
	void (*add)(struct pVoidArray*, void* data);

	//tools
	size_t (*size)(const struct pVoidArray*);
	void (*prepare)(struct pVoidArray*, size_t size);
} pVoidArrayInterface;

typedef struct pVoidArray {
	size_t size;
	size_t capacity;
	void** data;
	const pVoidArrayInterface* ops;
} pVoidArray;

pVoidArray pVoidArray_create();
pVoidArray* pVoidArray_new();
void pVoidArray_Delete(pVoidArray** arr);

#endif