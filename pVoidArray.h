#ifndef pVoidArray_header
#define pVoidArray_header

struct pVoidArray;

typedef struct {
	void* (*get)(struct pVoidArray*, int index);
	void* (*last)(struct pVoidArray*);
	void (*add)(struct pVoidArray*, void* data);
	int (*size)(struct pVoidArray*);
	void (*prepare)(struct pVoidArray*, int size);
} pVoidArrayInterface;

typedef struct pVoidArray {
	int size;
	int capacity;
	void** data;
	const pVoidArrayInterface* ops;
} pVoidArray;

pVoidArray pVoidArray_create();
pVoidArray* pVoidArray_new();
void pVoidArray_Delete(pVoidArray** arr);

#endif