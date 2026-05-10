#include <stdio.h>
#include "hashArray.h"
#include <string.h>

typedef struct pt {
	int x;
	int y;
} pt;

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

	int* qa = arr.ops->get(&arr,  key_a, strlen(key_a));
	int* qb = arr.ops->get(&arr, &key_b, sizeof(key_b));
	int* qc = arr.ops->get(&arr, &key_c, sizeof(key_c));

	printf("%d\t%d\t%d\n", *qa, *qb, *qc);
	return 0;
}