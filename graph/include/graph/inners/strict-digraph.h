#include "graph/strict-digraph.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// struct gr_edge {
//     void *to;
//     char ct[0];
// };
//
// struct gr_node {
//     struct gr_edge *edges; // []
//     char ct[0];
// };

#define EDGE_SIZE(g)        ((g)->edge_ct_size + sizeof(void *))
#define EDGES_ARRAY_SIZE(g) (EDGE_SIZE(g) * (g)->nedges)
#define NODE_SIZE(g)        ((g)->node_ct_size + EDGES_ARRAY_SIZE(g) + sizeof(size_t))

#define NODE_ID_PTR(g, n_ptr) ((size_t *)(n_ptr))
#define NODE_EDGE_PTR(g, n_ptr, e_idx) \
    ((void *)(NODE_ID_PTR(g, n_ptr) + 1) + (e_idx) * EDGE_SIZE(g))
#define NODE_CT_PTR(g, n) \
    ((unsigned *)(NODE_EDGE_PTR(g, n, 0) + EDGES_ARRAY_SIZE(g)))

#define NODE_EDGE_TO_PTR(g, n_ptr, e_idx) \
    (void **)(NODE_EDGE_PTR(g, n_ptr, e_idx))
#define NODE_EDGE_CT_PTR(g, n_ptr, e_idx) \
    (unsigned *)(NODE_EDGE_TO_PTR(g, n_ptr, e_idx) + 1)

#define EDGE_TO_PTR(g, e_ptr) \
    (void **)(e_ptr)
#define EDGE_CT_PTR(g, e_ptr) \
    (unsigned *)(EDGE_TO_PTR(g, e_ptr) + 1)

#define DESTROY_NODE_CT(g, n_ptr)                     \
    if ((g)->el_mtds.node_ct_destroy) {               \
        ((g)->el_mtds.node_ct_destroy(                \
            (g)->el_mtds.super, NODE_CT_PTR(g, n_ptr) \
        ));                                           \
    }

#define DESTROY_EDGE_CT(g, n_ptr, e_idx)                          \
    if ((g)->el_mtds.edge_ct_destroy) {                           \
        ((g)->el_mtds.edge_ct_destroy(                            \
            (g)->el_mtds.super, NODE_EDGE_CT_PTR(g, n_ptr, e_idx) \
        ));                                                       \
    }

#define IS_EMPTY_EDGE(g, n_ptr, e_idx) \
    (*NODE_EDGE_TO_PTR(g, n_ptr, e_idx) == NULL)

static inline size_t
find_empty_edge_id(const struct strict_digraph *g, void *node) {
    for (int i = 0; i < g->nedges; ++i) {
        if (IS_EMPTY_EDGE(g, node, i)) return i;
    }
    return g->nedges;
}

static inline size_t
find_edge_to(const struct strict_digraph *g, void *node, void *to) {
    for (int i = 0; i < g->nedges; ++i) {
        if (*NODE_EDGE_TO_PTR(g, node, i) == to) return i;
    }
    return g->nedges;
}

static inline void *new_node(struct strict_digraph *g, const void *node_ct) {
    void *res = calloc(1, NODE_SIZE(g));
    if (!res) return NULL;
    *NODE_ID_PTR(g, res) = g->cur_node_id++;
    memcpy(NODE_CT_PTR(g, res), node_ct, g->node_ct_size);
    memset(NODE_EDGE_PTR(g, res, 0), 0, EDGES_ARRAY_SIZE(g));
    return res;
}

static inline void destroy_node(const void *super, void *self) {
    const struct strict_digraph *g = super;
    void *node = *(void **)self;
    DESTROY_NODE_CT(g, node);
    for (int i = 0; i < g->nedges; ++i) { DESTROY_EDGE_CT(g, node, i); }
    free(node);
}
