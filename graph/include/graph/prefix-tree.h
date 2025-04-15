#pragma once
#include "common.h"
#include "vector/array.h"
#include <stddef.h>
#include <stdint.h>

#define PrefixTreeNode(node_key_type, node_val_type) \
    struct {                                         \
        Array(unsigned) edges;                       \
        node_key_type key;                           \
        node_val_type val;                           \
    } __attribute__((packed))

#define __PREFIX_TREE_BASICS \
    unsigned key_size;       \
    unsigned val_size;       \
    struct tree_el_manage el_mtds;

#define PrefixTree(key_type, val_type)                    \
    struct {                                              \
        Vector(PrefixTreeNode(key_type, val_type)) nodes; \
        __PREFIX_TREE_BASICS                              \
    }

#define PrefixTreePath(key_type, val_type) \
    struct {                               \
        const key_type *keys_arr;          \
        size_t nkeys;                      \
        val_type *value_ptr;               \
    }

#define NamedPrefixTreeNode(_typename, node_key_type, node_val_type) \
    struct _typename##_ptnode {                                      \
        Array(struct _typename##_ptnode *) edges;                    \
        node_key_type key;                                           \
        node_val_type val;                                           \
    } __attribute__((packed))

#define NamedPrefixTree(_typename)               \
    struct _typename {                           \
        Vector(struct _typename##_ptnode) nodes; \
        __PREFIX_TREE_BASICS                     \
    }

#define NamedPrefixTreePath(_typename, key_type, val_type) \
    struct _typename##_path_t {                            \
        const key_type *keys_arr;                          \
        size_t nkeys;                                      \
        val_type *value_ptr;                               \
    }

#define DECLARE_PREFIX_TREE(_typename, _key_type, _val_type)     \
    typedef NamedPrefixTreeNode(_typename, _key_type, _val_type) \
        _typename##_gnode;                                       \
    typedef NamedPrefixTree(_typename) _typename;                \
    typedef NamedPrefixTreePath(_typename, key_type, val_type);

struct prefix_tree;

struct prefix_tree_path {
    const void *keys_arr;
    size_t nkeys;
    void *value_ptr;
};

void prefix_tree_init(
    struct prefix_tree *tr,
    size_t key_size,
    size_t val_size,
    const struct tree_el_manage *el_mtds
);

void prefix_tree_free(struct prefix_tree *tr);
void prefix_tree_clear(struct prefix_tree *tr);

// добавляет путь согласно path.keys_arr,
// устанавливает path.value как значение по заданному пути, если такового ещё
// не существовало
// GR_ok -- путь успешно добавлен
// GR_key_error -- путь уже существует
// GR_memory_error -- ошибка выделения памяти
enum GR_STATUS prefix_tree_add_path(
    struct prefix_tree *tr, const struct prefix_tree_path *path
);

// удаляет путь соответствующий path.keys_arr.
// Значение path.value безразлично
// GR_ok -- путь успешно удалён
// GR_key_error -- путь не существует
enum GR_STATUS prefix_tree_remove_path(
    struct prefix_tree *tr, const struct prefix_tree_path *path
);

// ищет путь соответствующий path.keys_arr
// Записывает значнеие в path.value в случае, если путь существует
// GR_ok -- путь найден
// GR_key_error -- путь не найден
enum GR_STATUS
prefix_tree_find_path(struct prefix_tree *tr, struct prefix_tree_path *path);

#define P_TREE(tr) (struct prefix_tree *)(tr)

#define __P_TREE_TYPEOF_KEY(tr) typeof((tr)->nodes.data->key)
#define __P_TREE_TYPEOF_VAL(tr) typeof((tr)->nodes.data->val)

#define __P_TREE_SIZEOF_KEY(tr) sizeof((tr)->nodes.data->key)
#define __P_TREE_SIZEOF_VAL(tr) sizeof((tr)->nodes.data->val)

// clang-format off
#define P_TREE_INIT(tr, _key_cmp_eq, _key_destroy, _val_destroy, _super)   \
    {                                                                      \
        int (*cmp_key_fn)(                                                 \
                const typeof(*_super) *,                                   \
                const __P_TREE_TYPEOF_KEY(tr) *,                           \
                const __P_TREE_TYPEOF_KEY(tr) *) = _key_cmp_eq;            \
        void (*destroy_key_fn)(                                            \
                const typeof(*_super) *,                                   \
                __P_TREE_TYPEOF_KEY(tr) *) = _key_destroy;                 \
        void (*destroy_val_fn)(                                            \
                const typeof(*_super) *,                                   \
                __P_TREE_TYPEOF_VAL(tr) *) = _val_destroy;                 \
        struct tree_el_manage __mtds = {                                   \
            .key_cmp_eq = (                                                \
                    int (*)(const void *, const void *, const void *)      \
                ) cmp_key_fn,                                              \
            .key_destroy = (void (*)(const void *, void *))destroy_key_fn, \
            .val_destroy = (void (*)(const void *, void *))destroy_val_fn, \
            .super = _super,                                               \
        };                                                                 \
        prefix_tree_init(                                                  \
            P_TREE(tr),                                                    \
            __P_TREE_SIZEOF_KEY(tr),                                       \
            __P_TREE_SIZEOF_VAL(tr),                                       \
            &__mtds                                                        \
        );                                                                 \
    }
// clang-format on

#define P_TREE_ADD_PATH(status, tr, _key_seq, _key_seq_len, _value) \
    {                                                               \
        __P_TREE_TYPEOF_KEY(tr) *keys_arr_f = _key_seq;             \
        __P_TREE_TYPEOF_VAL(tr) value_f = _value;                   \
        struct prefix_tree_path __path = {                          \
            .keys_arr = keys_arr_f,                                 \
            .nkeys = _key_seq_len,                                  \
            .value_ptr = &value_f                                   \
        };                                                          \
        status = prefix_tree_add_path(P_TREE(tr), &__path);         \
    }

#define P_TREE_REMOVE_PATH(status, tr, _key_seq, _key_seq_len) \
    {                                                          \
        __P_TREE_TYPEOF_KEY(tr) *keys_arr_f = _key_seq;        \
        struct prefix_tree_path __path = {                     \
            .keys_arr = keys_arr_f,                            \
            .nkeys = _key_seq_len,                             \
            .value_ptr = NULL,                                 \
        };                                                     \
        status = prefix_tree_remove_path(P_TREE(tr), &__path); \
    }

#define P_TREE_FIND_PATH(status, tr, _key_seq, _key_seq_len, _value_ptr) \
    {                                                                    \
        __P_TREE_TYPEOF_KEY(tr) *keys_arr_f = _key_seq;                  \
        struct prefix_tree_path __path = {                               \
            .keys_arr = keys_arr_f,                                      \
            .nkeys = _key_seq_len,                                       \
            .value_ptr = NULL,                                           \
        };                                                               \
        status = prefix_tree_find_path(P_TREE(tr), &__path);             \
        _value_ptr = __path.value_ptr;                                   \
    }

#define P_TREE_FREE(tr) prefix_tree_free(P_TREE(tr));

#define P_TREE_CLEAR(tr) prefix_tree_clear(P_TREE(tr));

