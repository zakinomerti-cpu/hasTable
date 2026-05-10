#include "AvlMap.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




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
// tools
//-----------------------------------------------------------------------------
static size_t rotate_left(size_t value, char n, char r) {
	n %= r;
	return (value << n) | (value >> (r - n));
}

static char to_hex(char b) {
	if(b > 16) return -1;
	if(b < 10) {
		return '0' + b;
	}
	return 'A' + b - 10;
}

static char to_dec(char b) {
	if(b >= 'A' && b <= 'F') {
		return b - 'A' + 10;
	}
	if(b >= '0' && b <= '9') {
		return b - '0';
	}
	return -1;
}

static char* serialize(const void* key, size_t key_len) {
	if(!key || key_len == 0) return NULL;
	char* out = malloc(key_len*2 + 1);
	if(!out) return NULL;


	size_t strpos = 0;
	const uint8_t* b = (uint8_t*)key;
	for(int i = 0; i < key_len; i+=1) {
		char l = to_hex(b[i] & 0xF);
		char h = to_hex((rotate_left(b[i], 4, 8) & 0xF));
		out[strpos++] = h;
		out[strpos++] = l;
	}
	out[strpos] = '\0';

	return out;
}

static void* deserialize(const char* key) {
	if(!key) return NULL;
	size_t key_len = strlen(key);
	if (key_len % 2 != 0) return NULL; 


	uint8_t* out = malloc(key_len/2);
	if (!out) return NULL;

	uint8_t byte = 0;
	size_t strpos = 0;
	for(size_t i = 0; i < key_len / 2; i+=1) {
		char h = to_dec(key[strpos++]);
		char l = to_dec(key[strpos++]);

		if (h == -1 || l == -1) {
			free(out);
			return NULL;
		}

		byte = (uint8_t)h;
		byte = rotate_left(byte, 4, 8) + (uint8_t)l;
		out[i] = byte;
	}
	return (void*)out;
}




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

static void* get(node* p, const void* k, size_t kl) {
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

	return (void*)p->value;
}




//-----------------------------------------------------------------------------
//dynamic void array
//-----------------------------------------------------------------------------
size_t roots_count = 0;
static node** roots;


static void AvlMap_add(AvlMap* self, const void* key, size_t key_len, const void* value) {
	if(!self) return;
	if(self->id == -1) return;
	if(!roots[self->id]) {
		roots[self->id] = node_create(key, key_len, value);
		return;
	}
	node* n = roots[self->id];
	n = insert(n, key, key_len, value);
}

static void* AvlMap_get(AvlMap* self, const void* key, size_t key_len) {
	if(!self) return NULL;
	if(self->id == -1) return NULL;
	if(!roots[self->id]) {
		return NULL;
	}
	node* n = roots[self->id];
	return get(n, key, key_len);
}

static void AvlMap_set(AvlMap* self, 
	const void* key, size_t key_len, const void* value, size_t index) 
{
	if(!self) return;
	if(index >= roots_count) return;

	node* n = roots[index];
	insert(n, key, key_len, value);
}

static void AvlMap_prepare(AvlMap* self, size_t nsize) {
	if(!self) return;

	size_t old_count = roots_count;
	size_t new_count = roots_count + nsize;
	node** tmp = realloc(roots, sizeof(node*)*new_count);
	if(!tmp) return;
	roots = tmp;

	for(size_t i = old_count; i < new_count; i+=1) {
		roots[i] = NULL;
	}

	roots_count = new_count;
}

static const AvlMapInterface ops = {
	AvlMap_add,
	AvlMap_get,
	AvlMap_set,
	AvlMap_prepare
};

AvlMap AvlMap_create() {
	AvlMap arr;
	arr.size = 0;
	arr.capacity = 0;
	arr.ops = &ops;

	node** tmp = realloc(roots, sizeof(node*)*(roots_count+1));
	if(!tmp) arr.id = -1;
	tmp[roots_count] = NULL;
	roots = tmp;


	arr.id = roots_count++;
	return arr;
}

AvlMap* AvlMap_new() {
	AvlMap* arr = malloc(sizeof(AvlMap));
	if(!arr) return NULL;
	arr->size = 0;
	arr->capacity = 0;
	arr->ops = &ops;

	node** tmp = realloc(roots, sizeof(node*)*(roots_count+1));
	if(!tmp) arr->id = -1;
	tmp[roots_count] = NULL;
	roots = tmp;


	arr->id = roots_count++;
	return arr;
}



