#pragma once

#include "common/iterator.h"
#include "vector/vector.h"

#define PAIR(type1, type2) \
    struct {               \
        type1 first;       \
        type2 second;      \
    }

struct pair_manage {
    // методы принимают указатели на места расположения клюлчей
    size_t (*hash)(const void *);
    int (*key_cmp)(const void *, const void *);

    // optional:
    void (*destroy)(void *);
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

#define HASHTABLE_INIT(status, tb, _hash, _key_cmp, _destroy)       \
    {                                                               \
        struct pair_manage __mtds = {                               \
            .hash = _hash, .key_cmp = _key_cmp, .destroy = _destroy \
        };                                                          \
        status = hashtable_init(                                    \
            TB(tb),                                                 \
            sizeof((tb)->data.data->first),                         \
            sizeof((tb)->data.data->second),                        \
            &__mtds                                                 \
        );                                                          \
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
