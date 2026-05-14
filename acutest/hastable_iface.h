#ifndef __HASTABLE_IFACE_H__
#define __HASTABLE_IFACE_H__

#include <stddef.h>

typedef struct ht ht_t;

ht_t   *ht_create(size_t initial_capacity);
void    ht_destroy(ht_t *table);

/* Return codes:
    0 = OK,
    1 = duplicate,
   ‑1 = allocation failure */
int     ht_put(ht_t *table, const void *key, size_t key_len, const void *value);

/* returns NULL if key not found */
void   *ht_get(const ht_t *table, const void *key, size_t key_len);

/* return 0 on success, -1 if key absent */
int     ht_remove(ht_t *table,const void *key, size_t key_len);


extern size_t ht_size(const ht_t *table);
extern double ht_load_factor(const ht_t *table);

typedef void (*ht_iter_cb)(const void *key, size_t key_len, void *value, void *args);

/* iterator */
extern void ht_foreach(ht_t *table, ht_iter_cb cb, void *args);

#endif                          /* __HASTABLE_IFACE_H__ */
