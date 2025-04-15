#include "graph/prefix-tree.h"
#include "graph/common.h"
#include "vector/array.h"
#include "vector/common.h"
#include "vector/stack.h"
#include "vector/vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef Array(unsigned) conn_arr_t;
typedef Vector(unsigned) idx_vec_t;

#define NODE_SIZE(g) ((g)->key_size + (g)->val_size + sizeof(struct array))
#define NODE_IDX(g, n_ptr) \
    (((size_t)n_ptr - (size_t)(g)->nodes.data) / NODE_SIZE(g))
#define NODES_NUM(g) (VECTOR_SIZE(&(g)->nodes))

#define NODE_PTR(g, n_idx)        ((void *)VECTOR_ACCESS(&tr->nodes, n_idx))
#define NODE_CHILDS_PTR(g, n_ptr) ((conn_arr_t *)(n_ptr))
#define NODE_CHILDS_NUM(g, n_ptr) (ARRAY_SIZE(NODE_CHILDS_PTR(g, n_ptr)))
#define NODE_KEY_PTR(g, n_ptr)    ((void *)(NODE_CHILDS_PTR(g, n_ptr) + 1))
#define NODE_VAL_PTR(g, n_ptr) \
    ((void *)((size_t)NODE_KEY_PTR(g, n_ptr) + (g)->key_size))

#define ROOT_NODE(g) NODE_PTR(g, 0)

#define NODE_CONNECTED_ID(g, n_ptr, edge_id) \
    *(ARRAY_ACCESS(NODE_CHILDS_PTR(g, n_ptr), edge_id))

#define NODE_CONNECTED_PTR(g, n_ptr, edge_id) \
    (NODE_PTR(g, NODE_CONNECTED_ID(g, n_ptr, edge_id)))

#define DESTROY_NODE_KEY(g, n_ptr)                                            \
    if ((g)->el_mtds.key_destroy) {                                           \
        ((g)->el_mtds.key_destroy((g)->el_mtds.super, NODE_KEY_PTR(g, n_ptr)) \
        );                                                                    \
    }

#define DESTROY_NODE_VAL(g, n_ptr)                                            \
    if ((g)->el_mtds.val_destroy) {                                           \
        ((g)->el_mtds.val_destroy((g)->el_mtds.super, NODE_VAL_PTR(g, n_ptr)) \
        );                                                                    \
    }

#define PATH_EXISTS(tr, n_ptr) \
    ((n_ptr) != NULL && !is_zero(NODE_VAL_PTR(tr, n_ptr), (tr->val_size)))

struct ptree_node {
    conn_arr_t childs;
    uint8_t keyval[0];
};

struct prefix_tree {
    Vector(struct ptree_node) nodes;
    unsigned key_size;
    unsigned val_size;
    struct tree_el_manage el_mtds;
};

static inline int is_zero(const void *area, size_t size) {
    uint8_t res = 0;
    for (int i = 0; i < size; ++i) res |= *(uint8_t *)area++;
    return res == 0;
}

static inline unsigned node_edge_idx(
    const struct prefix_tree *tr, const void *node_ptr, const void *key
) {
    void *connected;
    const conn_arr_t *edges = NODE_CHILDS_PTR(tr, node_ptr);
    for (int i = 0; i < ARRAY_SIZE(edges); ++i) {
        connected = NODE_CONNECTED_PTR(tr, node_ptr, i);
        if (tr->el_mtds.key_cmp_eq(tr, key, NODE_KEY_PTR(tr, connected)))
            return i;
    }
    return NODE_CHILDS_NUM(g, node_ptr);
}

static inline void *node_connected_by(
    const struct prefix_tree *tr, const void *node_ptr, const void *key
) {
    const conn_arr_t *edges = NODE_CHILDS_PTR(tr, node_ptr);
    unsigned idx = node_edge_idx(tr, node_ptr, key);
    if (idx == NODE_CHILDS_NUM(g, node_ptr)) return NULL;
    return NODE_CONNECTED_PTR(tr, node_ptr, idx);
}

static inline int
connect_nodes(struct prefix_tree *tr, void *node_ptr, void *to_ptr) {
    const conn_arr_t *edges = NODE_CHILDS_PTR(tr, node_ptr);
    if (array_resize(ARR(edges), ARRAY_SIZE(edges) + 1) != VEC_ok) return 0;
    *ARRAY_ACCESS_RIHGT(edges, 0) = NODE_IDX(tr, to_ptr);
    return 1;
}

static inline int remove_edge(struct prefix_tree *tr, void *node_ptr, int idx) {
    conn_arr_t *edges = NODE_CHILDS_PTR(tr, node_ptr);
    memmove(
        ARRAY_ACCESS(edges, idx),
        ARRAY_ACCESS(edges, idx + 1),
        (ARRAY_SIZE(edges) - idx - 1) / ARRAY_EL_SIZE(edges)
    );
    if (array_resize(ARR(edges), ARRAY_SIZE(edges) - 1) != VEC_ok) return 0;
    return 1;
}

static inline int
unconnect_nodes(struct prefix_tree *tr, void *node_ptr, void *to_ptr) {
    conn_arr_t *edges = NODE_CHILDS_PTR(tr, node_ptr);
    size_t idx = node_edge_idx(tr, node_ptr, NODE_KEY_PTR(tr, to_ptr));
    return remove_edge(tr, node_ptr, idx);
}

static inline void *find_path(
    const struct prefix_tree *tr,
    const struct prefix_tree_path *path,
    void **prev,
    void **res_key_ptr
) {
    void *cur_node, *prev_node;
    size_t nnodes = VECTOR_SIZE(&tr->nodes);
    const void *key_ptr = path->keys_arr;
    size_t nkeys = path->nkeys;

    cur_node = ROOT_NODE(tr);

    while (nkeys--) {
        prev_node = cur_node;
        if ((cur_node = node_connected_by(tr, cur_node, key_ptr)) == NULL)
            break;
        key_ptr += tr->key_size;
    }
    if (res_key_ptr) *res_key_ptr = (void *)key_ptr;
    if (prev) *prev = prev_node;
    return cur_node;
}

void node_default_init(const void *super, void *node_ptr) {
    struct el_manage mng = {};
    const struct prefix_tree *tr = super;

    array_init(ARR(NODE_CHILDS_PTR(tr, node_ptr)), sizeof(unsigned), &mng);
    memset(NODE_VAL_PTR(tr, node_ptr), 0, tr->val_size);
}

void node_destroy(const void *super, void *node_ptr) {
    const struct prefix_tree *tr = super;

    DESTROY_NODE_KEY(tr, node_ptr);
    DESTROY_NODE_VAL(tr, node_ptr);
    ARRAY_FREE(NODE_CHILDS_PTR(tr, node_ptr));
}

// ОСТОРОЖНО!!!!
// при вызове этой функции потенциально рпоисходит realloc памяти, где
// располагаются узлы. Указатели, существовавшие до вызова функции могут
// перестать быть валидными
static inline void *
new_node(struct prefix_tree *tr, const void *key, const void *val) {
    struct el_manage mng = {};
    void *new;

    if (vector_alloc_back(VEC(&tr->nodes), 1) != VEC_ok) return NULL;
    new = VECTOR_ACCESS_RIHGT(&tr->nodes, 0);

    if (key) memcpy(NODE_KEY_PTR(tr, new), key, tr->key_size);
    if (val) memcpy(NODE_VAL_PTR(tr, new), val, tr->val_size);
    return new;
}

// удаляет узлы в диапазоне [start, end)
// корректирует значения стрелок, указывавших на узлы начиная с end
// не удаляет стрелки ни на какие узлы из диапазона
static inline void
remove_nodes_range(struct prefix_tree *tr, void *start, void *end) {
    const conn_arr_t *edges;
    size_t end_idx = NODE_IDX(tr, end);
    size_t remove_num = (end - start) / NODE_SIZE(tr);

    // TODO: Тут вообще надо бы циклический сдвиг сделать. Тогда не придётся
    // обманывать вектр. Да и вообще, не факт, что memset на 0 - лучшее
    // решение. Но опусть пока так останется. Я устал ;(
    if (remove_num) {
        // разрушаем узлы в диапазоне, так как вектор это автоматически не
        // сделает
        for (void *cur = start; cur != end; cur += NODE_SIZE(tr))
            node_destroy(tr, cur);
        // перемещаем узлы стоящие начиная с end на позицию start
        memmove(start, end, (NODES_NUM(tr) - end_idx) * NODE_SIZE(tr));
        // зануляем освободившуюс память в конце, чтобы вектор не разрушил
        // эти элементы
        memset(
            VECTOR_ACCESS_RIHGT(&tr->nodes, remove_num - 1),
            0,
            remove_num * NODE_SIZE(tr)
        );
        // укорачиваем вектор
        vector_resize(VEC(&tr->nodes), NODES_NUM(tr) - remove_num);
    }

    for (int i = 0; i < NODES_NUM(tr); ++i) {
        edges = NODE_CHILDS_PTR(tr, NODE_PTR(tr, i));
        for (int j = 0; j < ARRAY_SIZE(edges); ++j) {
            if (*ARRAY_ACCESS(edges, j) >= end_idx)
                *ARRAY_ACCESS(edges, j) -= remove_num;
        }
    }
}

void prefix_tree_init(
    struct prefix_tree *tr,
    size_t key_size,
    size_t val_size,
    const struct tree_el_manage *el_mtds
) {
    struct el_manage mng;

    mng = (struct el_manage){
        .super = tr,
        .default_init = node_default_init,
        .destroy = node_destroy,
    };

    tr->key_size = key_size;
    tr->val_size = val_size;
    tr->el_mtds = *el_mtds;
    vector_init(VEC(&tr->nodes), NODE_SIZE(tr), &mng);
    new_node(tr, NULL, NULL);
}

void prefix_tree_free(struct prefix_tree *tr) {
    if (!tr) return;
    vector_free(VEC(&tr->nodes));
}

void prefix_tree_clear(struct prefix_tree *tr) {
    vector_clear(VEC(&tr->nodes));
}

enum GR_STATUS
prefix_tree_find_path(struct prefix_tree *tr, struct prefix_tree_path *path) {
    void *cur_node = find_path(tr, path, NULL, NULL);

    if (!PATH_EXISTS(tr, cur_node)) return GR_key_error;

    path->value_ptr = NODE_VAL_PTR(tr, cur_node);
    return GR_ok;
}

enum GR_STATUS prefix_tree_add_path(
    struct prefix_tree *tr, const struct prefix_tree_path *path
) {
    void *prev, *key_ptr, *new;
    void *cur_node = find_path(tr, path, &prev, &key_ptr);
    size_t nkeys =
        path->nkeys - ((size_t)key_ptr - (size_t)path->keys_arr) / tr->key_size;
    size_t cur_idx;

    if (PATH_EXISTS(tr, cur_node)) return GR_key_error;
    if (!cur_node) {
        cur_idx = NODE_IDX(tr, prev);
        while (nkeys--) {
            // тут могут остаться висячие ноды, но мне видится, что это не
            // проблема, т.к. на поиск пути это не повлияет (значения всё равно
            // нет) а при повторной вставке того же ключа, если памяти хватит,
            // то всё пройдёт бесшовно
            if (!(new = new_node(tr, key_ptr, NULL))) return GR_memory_error;
            if (!connect_nodes(tr, NODE_PTR(tr, cur_idx), new))
                return GR_memory_error;
            key_ptr += tr->key_size;
            cur_idx = NODE_IDX(tr, new);
        }
        cur_node = NODE_PTR(tr, cur_idx);
    }
    memcpy(NODE_VAL_PTR(tr, cur_node), path->value_ptr, tr->val_size);
    return GR_ok;
}

enum GR_STATUS prefix_tree_remove_path(
    struct prefix_tree *tr, const struct prefix_tree_path *path
) {
    enum GR_STATUS res = GR_ok;
    enum VEC_STATUS vstt;
    void *end_node, *prev_node, *rm_start = NULL;
    void *cur_node = ROOT_NODE(tr);
    const void *key_ptr = path->keys_arr;
    unsigned idx;

    end_node = find_path(tr, path, NULL, NULL);
    if (!PATH_EXISTS(tr, end_node)) return GR_key_error;

    // убираем значение в конечном узле маршрута в любом случае
    DESTROY_NODE_VAL(tr, end_node);
    memset(NODE_VAL_PTR(tr, end_node), 0, tr->val_size);

    // если у конечного узла пути есть потомки, значит нет смысла начинать
    // алгоритм поиска начала удаляемой последовательности
    if (NODE_CHILDS_NUM(tr, end_node) != 0) return GR_ok;

    while (cur_node != end_node) {
        cur_node = node_connected_by(tr, cur_node, key_ptr);
        if (!rm_start) rm_start = cur_node;

        // если встречен узел с более чем одним потомком, или узел, хранящий
        // значение, он станет предшествующим удаляемым узлам
        if (NODE_CHILDS_NUM(tr, cur_node) > 1 || PATH_EXISTS(tr, cur_node)) {
            prev_node = cur_node;
            rm_start = NULL;
        }
        key_ptr += tr->key_size;
    }

    if (!unconnect_nodes(tr, prev_node, rm_start)) goto mem_err;

    // удаляем все узлы в диапазоне [rm_start, prev_node]
    // это можно сделать, так как удаляться может только несколько
    // последовательно идущих узлов без ветвления
    remove_nodes_range(tr, rm_start, end_node + NODE_SIZE(tr));

end:
    return res;
mem_err:
    res = GR_memory_error;
    goto end;
}
