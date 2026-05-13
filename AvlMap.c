#include "AvlMap.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "serialize_to_bytes.h"




//-----------------------------------------------------------------------------
// typedefs
//-----------------------------------------------------------------------------
typedef struct node {
	char* key;
	uint8_t height;
	struct node* left;
	struct node* right;
	const void* value;
} node;




//-----------------------------------------------------------------------------
// avl nodes
//-----------------------------------------------------------------------------
static node* node_create(const void* key, size_t key_len, const void* value) {
	node* n = malloc(sizeof(node));
	n->key = serialize(key, key_len);
	n->height = 1;
	n->left = NULL;
	n->right = NULL;
	n->value = value;
	return n;
}

static uint8_t height(node* p) {
	return p ? p->height : 0;
}

static int bfactor(node* p) {
	return height(p->right)-height(p->left);
}

static void fixheight(node* p) {
	uint8_t hl = height(p->left);
	uint8_t hr = height(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}

static node* rotateright(node* p) {
	node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}

static node* rotateleft(node* q) {
	node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}

static node* balance(node* p) {
	fixheight(p);
	if( bfactor(p)==2 )
	{
		if( bfactor(p->right) < 0 )
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if( bfactor(p)==-2 )
	{
		if( bfactor(p->left) > 0  )
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p;
}

static node* insert(node* p, const void* k, size_t kl, const void* v) {
	if( !p ) {
		return node_create(k, kl, v);
	}

	char* input_key = serialize(k, kl);
	char* local_key = p->key;
	int compare_result = strcmp(input_key, local_key);
	if( compare_result < 0)
		p->left = insert(p->left, k, kl, v);
	else
		p->right = insert(p->right, k, kl, v);

	free(input_key);
	return balance(p);
}

static const void* get(node* p, const void* k, size_t kl) {
	if( !p ) {
		return NULL;
	}

	char* input_key = serialize(k, kl);
	char* local_key = p->key;
	int compare_result = strcmp(input_key, local_key);
	free(input_key);

	if( compare_result < 0 ) {
		return get(p->left, k, kl);
	}

	if( compare_result > 0 ) {
		return get(p->right, k, kl);
	}

	return p->value;
}




//-----------------------------------------------------------------------------
// flat array
//-----------------------------------------------------------------------------

key_value_t* key_value_t_create(char* key, void* value) {
	key_value_t* t = malloc(sizeof(key_value_t));
	if (!t) return NULL;
	t->key = key;
	t->value = value;
	return t;
}

void flatNodeArr_add(flatNodeArr** a, char* key, void* value) {
	if (!*a) {
		*a = malloc(sizeof(flatNodeArr));
		if (!*a) return;
		(*a)->keyArr = malloc(sizeof(flatNodeArr*));
		(*a)->keyArr[0] = key_value_t_create(key, value);
		(*a)->count = 1;
		return;
	}

	key_value_t** tmp = realloc((*a)->keyArr, sizeof(key_value_t*)*((*a)->count+1));
	if (!tmp) return; (*a)->keyArr = tmp;
	(*a)->keyArr[(*a)->count] = key_value_t_create(key, value);
	(*a)->count+=1;

}

void flatNodeArr_get_all(flatNodeArr** arr, node* p) {
	if( !p ) {
		return;
	}
	if (!p->left && !p->right) {
		flatNodeArr_add(arr, p->key, (void* const)p->value);
		return;
	}

	flatNodeArr_add(arr, p->key, (void* const)p->value);
	flatNodeArr_get_all(arr, p->left);
	flatNodeArr_get_all(arr, p->right);
}

void flatNodeArr_delete(flatNodeArr** arr) {
	free((*arr)->keyArr); free(*arr); *arr = NULL;
}




//-----------------------------------------------------------------------------
//dynamic avl map
//-----------------------------------------------------------------------------
static void AvlMap_add(AvlMap* self, const void* key, size_t key_len, const void* value) {
	if(!self) return;
	self->size+=1;
	self->root = insert(self->root, key, key_len, value);
}

static void* AvlMap_get(AvlMap* self, const void* key, size_t key_len) {
	if(!self || !self->root) return NULL;
	return (void* const)get(self->root, key, key_len);
}

static void AvlMap_getAll(AvlMap* self, flatNodeArr** arr) {
	if (!self) return;
	flatNodeArr_get_all(arr, self->root);
}



static const AvlMapInterface ops = {
	AvlMap_add,
	AvlMap_get,
	AvlMap_getAll
};

AvlMap* AvlMap_new() {
	AvlMap* arr = malloc(sizeof(AvlMap));
	if(!arr) return NULL;
	arr->size = 0;
	arr->root = NULL;
	arr->ops = &ops;
	return arr;
}



