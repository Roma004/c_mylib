#include "common/iterator.h"
#include "hashtable/hashtable-inners.h"
#include "hashtable/hashtable.h"

void table_iter_get(const Iterator *it, void *dst);
void table_iter_set(Iterator *it, const void *src);
void table_iter_inc(Iterator *it);
int table_iter_cmp(const Iterator *a, const Iterator *b);
void table_iter_free(Iterator *it);
void table_iter_copy(const Iterator *it, Iterator *dst);

struct iterator_vtable hashtable_iter_methods = {
    .ref = NULL,
    .get = table_iter_get,
    .set = table_iter_set,
    .inc = table_iter_inc,
    .dec = NULL,
    .cmp = table_iter_cmp,
    .free = table_iter_free,
    .copy = table_iter_copy,
};

void hashtable_iter_init(const HashTable *tb, HashTableIterator *it, int idx) {
    if (!it) return;
    it->tb = (HashTable *)tb;
    it->cur_pos = idx;
    ITER_VTABLE_INIT(it, &hashtable_iter_methods);
}

void table_iter_get(const Iterator *iter, void *dst) {
    const HashTableIterator *it = (const HashTableIterator *)iter;
    if (!it) return;
    memcpy(
        dst,
        TB_NODE_PTR(it->tb, it->cur_pos),
        it->tb->key_size + it->tb->val_size
    );
}

void table_iter_set(Iterator *iter, const void *src) {
    HashTableIterator *it = (HashTableIterator *)iter;
    if (!it) return;
    memcpy(
        TB_NODE_PTR(it->tb, it->cur_pos),
        src,
        it->tb->key_size + it->tb->val_size
    );
}

void table_iter_inc(Iterator *iter) {
    HashTableIterator *it = (HashTableIterator *)iter;
    it->cur_pos = next_idx(it->tb, it->cur_pos);
}

int table_iter_cmp(const Iterator *a_it, const Iterator *b_it) {
    HashTableIterator *a = (HashTableIterator *)a_it;
    HashTableIterator *b = (HashTableIterator *)b_it;
    return memcmp(a, b, sizeof(HashTableIterator)) == 0;
}

void table_iter_copy(const Iterator *iter, Iterator *dst_it) {
    HashTableIterator *it = (HashTableIterator *)iter;
    HashTableIterator *dst = (HashTableIterator *)dst_it;
    *dst = *it;
}

void table_iter_free(Iterator *it) {}
