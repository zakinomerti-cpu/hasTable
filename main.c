#include <stdio.h>
#include "hashArray.h"
#include <string.h>

#include "AvlMap.h"

typedef struct pt {
	int x;
	int y;
} pt;

void callback(const void *key, size_t key_len, void *value, void *args) { //калбакишончик
	int* a = (int*)value;
	*a *= 2;
	printf("%d\n", *a);
}


int main() {


	int a = 5;
	int b = 6;
	int c = 7;
	char* key_a = "key1";
	short key_b = 12345;
	pt key_c; key_c.x = 10; key_c.y = 20;
	hashArray arr = hashArray_create(1024);
	arr.ops->add(&arr,  key_a, strlen(key_a), &a);
	arr.ops->add(&arr, &key_b, sizeof(key_b), &b);
	arr.ops->add(&arr, &key_c, sizeof(key_c), &c);

	arr.ops->resize(&arr);

	int* qa = arr.ops->get(&arr,  key_a, strlen(key_a));
	int* qb = arr.ops->get(&arr, &key_b, sizeof(key_b));
	int* qc = arr.ops->get(&arr, &key_c, sizeof(key_c));

	AvlMap* map = AvlMap_new();
	map->ops->add(map, &b, sizeof(int), &a);
	map->ops->add(map, &a, sizeof(int), &b);
	flatNodeArr* flat = NULL;
	map->ops->getAll(map, &flat);

	ht_iter_cb cb = callback;
	arr.ops->remove(&arr, key_a, strlen(key_a));
	arr.ops->forEach(&arr, cb, NULL);
	return 0;
}