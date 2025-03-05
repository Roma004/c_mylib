#pragma once
#include "common/iterator.h"
#include "hashtable/hashtable.h"
#include "vector/vector.h"
#include <stdlib.h>
#include <string.h>

extern struct iterator_vtable hashtable_iter_methods;

enum busy { FREE = 0, BUSY, DEL };

#define NODE_SIZE(tb) ((tb)->key_size + (tb)->val_size + sizeof(enum busy))

#define NODE_KEY_PTR(tb, node) (void *)(node)
#define NODE_VAL_PTR(tb, node)  (void *)((uint8_t *)(node) + (tb)->key_size)
#define NODE_BUSY_PTR(tb, node) \
    (enum busy *)((uint8_t *)(node) + (tb)->key_size + (tb)->val_size)

#define TB_NODE_PTR(tb, idx)      (void *)VECTOR_ACCESS(&(tb)->data, idx)
#define TB_NODE_KEY_PTR(tb, idx)  NODE_KEY_PTR(tb, TB_NODE_PTR(tb, idx))
#define TB_NODE_VAL_PTR(tb, idx)  NODE_VAL_PTR(tb, TB_NODE_PTR(tb, idx))
#define TB_NODE_BUSY_PTR(tb, idx) NODE_BUSY_PTR(tb, TB_NODE_PTR(tb, idx))

#define TB_CAPACITY(tb) (tb)->data.capacity

#define MAX_FILL_FACTOR (double)2 / 3
#define MIN_FILL_FACTOR (double)1 / 2
#define MULTIPLIER      2
#define DIVISOR         2
#define MIN_CAPACITY    10
#define STEP            13

void hashtable_iter_init(const HashTable *tb, HashTableIterator *it, int idx);

static inline size_t hash(const HashTable *tb, const void *key);
static inline void place_node(HashTable *tb, const void *node);
static inline enum TB_STATUS table_expand(HashTable *tb);
static inline enum TB_STATUS table_shrink(HashTable *tb);
static inline void
emplace_node(HashTable *tb, int idx, const void *key, const void *val);

static inline void reset_data(HashTable *tb) {
    memset(tb->data.data, 0, TB_CAPACITY(tb) * NODE_SIZE(tb));
}

static inline size_t hash(const HashTable *tb, const void *key) {
    return tb->mtds.hash(key) % TB_CAPACITY(tb);
}

// значение по указателю будет скопировано
// никаких проверок на валидность не происходит
static inline void place_node(HashTable *tb, const void *node) {
    vector_set(VEC(&tb->data), node, hash(tb, NODE_KEY_PTR(tb, node)));
    tb->nnodes += 1;
}

// значение по указателю будет скопировано
// никаких проверок на валидность не происходит
static inline void
emplace_node(HashTable *tb, int idx, const void *key, const void *val) {
    void *node = TB_NODE_PTR(tb, idx);
    memcpy(NODE_KEY_PTR(tb, node), key, tb->key_size);
    memcpy(NODE_VAL_PTR(tb, node), val, tb->val_size);
    *NODE_BUSY_PTR(tb, node) = BUSY;
    tb->nnodes += 1;
}

static inline enum TB_STATUS table_expand(HashTable *tb) {
    Vector(void) new_data, tmp;
    enum VEC_STATUS v_stt;
    vector_init(VEC(&new_data), tb->data.el_size, &tb->data.el_mtds);
    if ((v_stt = vector_alloc_back(VEC(&new_data), TB_CAPACITY(tb) * MULTIPLIER))
        != VEC_ok)
        return TB_memory_error;

    memcpy(&tmp, &new_data, sizeof(new_data));
    memcpy(&new_data, &tb->data, sizeof(new_data));
    memcpy(&tb->data, &tmp, sizeof(new_data));

    reset_data(tb);
    tb->nnodes = 0;
    for (int i = 0; i < new_data.capacity; ++i) {
        void *cur_node = VECTOR_ACCESS(&new_data, i);
        if (*NODE_BUSY_PTR(tb, cur_node) == BUSY) place_node(tb, cur_node);
    }

    new_data.el_mtds.destroy = NULL;
    vector_free(VEC(&new_data));
    return TB_ok;
}

static inline enum TB_STATUS table_shrink(HashTable *tb) {
    Vector(void) new_data, tmp;
    enum VEC_STATUS v_stt;
    vector_init(VEC(&new_data), tb->data.el_size, &tb->data.el_mtds);
    if ((v_stt = vector_alloc_back(VEC(&new_data), TB_CAPACITY(tb) / DIVISOR))
        != VEC_ok)
        return TB_memory_error;

    memmove(&tmp, &new_data, sizeof(new_data));
    memmove(&new_data, &tb->data, sizeof(new_data));
    memmove(&tb->data, &tmp, sizeof(new_data));

    reset_data(tb);
    tb->nnodes = 0;
    for (int i = 0; i < new_data.capacity; ++i) {
        void *cur_node = VECTOR_ACCESS(&new_data, i);
        if (*NODE_BUSY_PTR(tb, cur_node) == BUSY) place_node(tb, cur_node);
    }

    new_data.el_mtds.destroy = NULL;
    vector_free(VEC(&new_data));
    return TB_ok;
}

static inline size_t next_idx(const HashTable *tb, size_t idx) {
    if (idx >= TB_CAPACITY(tb)) return TB_CAPACITY(tb);
    ++idx;

    while (idx < TB_CAPACITY(tb) && *TB_NODE_BUSY_PTR(tb, idx) != BUSY) idx++;

    return idx;
}
