#include "hastable_iface.h"
#include "acutest.h"

#include <string.h>
#include <stdint.h>

static void
count_cb(const void *key, size_t key_len, void *value, void *user)
{
    (void)key; (void)key_len; (void)value;
    size_t *cnt = (size_t *)user;
    (*cnt)++;
}

void
test_empty_table(void)
{
    ht_t *ht = ht_create(4);
    TEST_ASSERT(ht);
    TEST_CHECK(ht_size(ht) == 0);
    TEST_CHECK(ht_get(ht, "a", 1) == NULL);

    int q = 0;
    TEST_CHECK((q = ht_remove(ht, "a", 1)) == -1);
    ht_destroy(ht);
}

void
test_put_and_get(void)
{
    ht_t *ht = ht_create(4);
    int v = 123;
    TEST_CHECK(ht_put(ht, "a", 1, &v) == 0);
    TEST_CHECK(ht_size(ht) == 1);
    TEST_CHECK(*(int *)ht_get(ht, "a", 1) == 123);
    ht_destroy(ht);
}

void
test_duplicate_key(void)
{
    ht_t *ht = ht_create(4);
    int v1 = 1, v2 = 2;
    TEST_CHECK(ht_put(ht, "x", 1, &v1) == 0);
    TEST_CHECK(ht_put(ht, "x", 1, &v2) == 1);
    TEST_CHECK(*(int *)ht_get(ht, "x", 1) == 1);
    ht_destroy(ht);
}

void
test_resize_and_load_factor(void)
{
    ht_t *ht = ht_create(2);

    double lf_before = ht_load_factor(ht);

    /* insert many keys to force changes in capacity and load factor */
    const size_t N = 100;
    for (size_t i = 0; i < N; ++i) {
        char key[8];
        int  val = (int)i;
        memcpy(key, &i, sizeof i < 8 ? sizeof i : 8);
        TEST_CHECK(ht_put(ht, key, 8, &val) == 0);
    }

    TEST_CHECK(ht_size(ht) == N);

    double lf_after = ht_load_factor(ht);
    TEST_CHECK(lf_after != lf_before);

    ht_destroy(ht);
}

void
test_remove_and_foreach(void)
{
    ht_t *ht = ht_create(4);
    int v1 = 10, v2 = 20;
    ht_put(ht, "a", 1, &v1);
    ht_put(ht, "b", 1, &v2);

    TEST_CHECK(ht_remove(ht, "a", 1) == 0);
    TEST_CHECK(ht_get(ht, "a", 1) == NULL);
    TEST_CHECK(ht_size(ht) == 1);

    size_t cnt = 0;
    ht_foreach(ht, count_cb, &cnt);
    TEST_CHECK(cnt == 1);
    ht_destroy(ht);
}

TEST_LIST = {
    { "empty_table",        test_empty_table },
    { "put_and_get",        test_put_and_get },
    { "duplicate_key",      test_duplicate_key },
    { "resize_and_load",    test_resize_and_load_factor },
    { "remove_and_foreach", test_remove_and_foreach },
    { NULL, NULL }
};
