#pragma once

#include "common/iterator.h"
#include "common/common.h"
#include "vector/vector.h"


struct pair_manage {
    const void *super;
    // методы принимают указатели на места расположения клюлчей
    size_t (*hash)(const void *super, const void *key);
    int (*key_cmp)(const void *super, const void *key_a, const void *key_b);

    // optional:
    void (*destroy_key)(const void *super, void *self);
    void (*destroy_value)(const void *super, void *self);
};

#define __HASHTABLE_FIELDS(type) \
    Vector(type) data;           \
    size_t key_size;             \
    size_t val_size;             \
    size_t nnodes;               \
    struct pair_manage mtds;

#define HashTable(type1, type2)                \
    struct {                                   \
        __HASHTABLE_FIELDS(PAIR(type1, type2)) \
    }

#define TB(tb) (struct hashtable *)(tb)

#define TB_KEY_TYPE(tb) (typeof((tb)->data.data->first)
#define TB_VAL_TYPE(tb) (typeof((tb)->data.data->second)

#define HASHTABLE_INIT(                                                          \
    status, tb, _hash, _key_cmp, _destroy_key, _destory_val, _super              \
)                                                                                \
    {                                                                            \
        void (*destroy_key_fn)(const void *, TB_KEY_TYPE(tb) *) =                \
            _destroy_key;                                                        \
        void (*destroy_val_fn)(const void *, TB_VAL_TYPE(tb) *) =                \
            _destroy_val;                                                        \
        void (*key_cmp_fn)(const void *, TB_KEY_TYPE(tb) *, TB_KEY_TYPE(tb) *) = \
            _key_cmp;                                                            \
        void (*hash_fn)(const void *, TB_KEY_TYPE(tb) *) = _key_cmp;             \
        struct pair_manage __mtds = {                                            \
            .super = _super,                                                     \
            .hash = hash_fn,                                                     \
            .key_cmp = key_cmp_fn,                                               \
            .destroy_key = destroy_key_fn,                                       \
            .destroy_value = destroy_val_fn,                                     \
        };                                                                       \
        status = hashtable_init(                                                 \
            TB(tb),                                                              \
            sizeof((tb)->data.data->first),                                      \
            sizeof((tb)->data.data->second),                                     \
            &__mtds                                                              \
        );                                                                       \
    }

#define HASHTABLE_INSERT(status, tb, _key, _val, iter)       \
    {                                                        \
        typeof((tb)->data.data->first) key = _key;           \
        typeof((tb)->data.data->second) val = _val;          \
        status = hashtable_insert(TB(tb), &key, &val, iter); \
    }

#define HASHTABLE_FIND(status, tb, _key, iter)       \
    {                                                \
        typeof((tb)->data.data->first) key = _key;   \
        status = hashtable_find(TB(tb), &key, iter); \
    }

#define HASHTABLE_ERASE(status, tb, _key, iter_next)      \
    {                                                     \
        typeof((tb)->data.data->first) key = _key;        \
        status = hashtable_find(TB(tb), &key, iter_next); \
    }

typedef struct hashtable {
    __HASHTABLE_FIELDS(void)
} HashTable;

typedef struct hashtable_itarator {
    ITERATOR_BASE
    struct hashtable *tb;
    size_t cur_pos;
} HashTableIterator;

enum TB_STATUS {
    TB_ok = 0,
    TB_key_error,
    TB_memory_error,
};

enum TB_STATUS hashtable_init(
    HashTable *tb, size_t key_size, size_t val_size, struct pair_manage *mtds
);

enum TB_STATUS hashtable_insert(
    HashTable *tb, const void *key, const void *val, HashTableIterator *ins_pos
);

enum TB_STATUS
hashtable_erase(HashTable *tb, const void *key, HashTableIterator *next);

void hashtable_erase_by_iter(HashTableIterator *it);

enum TB_STATUS
hashtable_find(const HashTable *tb, const void *key, HashTableIterator *pos);

void hashtable_free(HashTable *tb);

void hashtable_begin(const HashTable *tb, HashTableIterator *it);
void hashtable_end(const HashTable *tb, HashTableIterator *it);
