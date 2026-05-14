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
static node* node_create(char* key, const void* value) {
	node* n = malloc(sizeof(node));
	n->key = key;
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

static int insert(node** p, char** key, const void* v) {
	if( !*p ) {
		*p = node_create(*key, v);
		if (!*p) return -1;
		return 0;
	}

	int out = 0;
	int compare_result = strcmp((*p)->key, *key);

	if( compare_result < 0)
		out = insert(&(*p)->left, key, v);
	else if ( compare_result > 0) {
		out = insert(&(*p)->right, key, v);
	}
	else { //duplicate
		free(*key);
		*key = NULL;
		return 1;
	}

	*p = balance(*p);
	return out;
}

static const void* get(node* p, const char* key) {
	if( !p ) {
		return NULL;
	}

	int compare_result = strcmp(p->key, key);

	if( compare_result < 0 ) {
		return get(p->left, key);
	}

	if( compare_result > 0 ) {
		return get(p->right, key);
	}

	return p->value;
}

static const void** get_double_indirect(node* p, const char* key) {
	if( !p ) {
		return NULL;
	}

	int compare_result = strcmp(p->key, key);

	if( compare_result < 0 ) {
		return get_double_indirect(p->left, key);
	}

	if( compare_result > 0 ) {
		return get_double_indirect(p->right, key);
	}

	return &p->value;
}

static node* findmin(node* p) // поиск узла с минимальным ключом в дереве p
{
	return p->left?findmin(p->left):p;
}

static node* removemin(node* p) // удаление узла с минимальным ключом из дерева p
{
	if( p->left==0 )
		return p->right;
	p->left = removemin(p->left);
	return balance(p);
}

static node* node_remove(node* p, const char* key)
{
	if( !p ) return 0;

	const int k = strcmp(p->key, key);
	if( k < 0 )
		p->left = node_remove(p->left,key);
	else if( k > 0 )
		p->right = node_remove(p->right,key);
	else //  k == p->key
	{
		node* q = p->left;
		node* r = p->right;

		free(p->key);
		free(p);


		if( !r ) return q;
		node* min = findmin(r);
		min->right = removemin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
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
static int AvlMap_add(AvlMap* self, const void* key, size_t key_len, const void* value) {
	if(!self) return -2;

	char* str_key = serialize(key, key_len);
	int out = insert(&self->root, &str_key, value);
	self->size+=1;
	return out;
}

static void* AvlMap_get(AvlMap* self, const void* key, size_t key_len) {
	if(!self || !self->root) return NULL;
	char* str_key = serialize(key, key_len);
	void* const out = (void* const)get(self->root, str_key);
	free(str_key);
	return out;
}

static void** AvlMap_get_double_indirect(AvlMap* self,
	const void* key, size_t key_len)
{
	if(!self || !self->root) return NULL;
	char* str_key = serialize(key, key_len);
	void** const out = (void* const)get_double_indirect(self->root, str_key);
	free(str_key);
	return out;
}

static void AvlMap_getAll(AvlMap* self, flatNodeArr** arr) {
	if (!self) return;
	flatNodeArr_get_all(arr, self->root);
}

static int AvlMap_remove(AvlMap* self, const void* key, size_t key_len) {
	if (!self) return -1;
	char* str_key = serialize(key, key_len);
	self->root = node_remove(self->root, str_key);
	free(str_key);
	self->size -=1;
	return 0;
}



static const AvlMapInterface ops = {
	AvlMap_add,
	AvlMap_get,
	AvlMap_get_double_indirect,
	AvlMap_getAll,
	AvlMap_remove
};

AvlMap* AvlMap_new() {
	AvlMap* arr = malloc(sizeof(AvlMap));
	if(!arr) return NULL;
	arr->size = 0;
	arr->root = NULL;
	arr->ops = &ops;
	return arr;
}



