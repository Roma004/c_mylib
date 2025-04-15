#include "graph/strict-digraph.h"
#include "graph/inners/strict-digraph.h"
#include "vector/vector.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void strict_digraph_init(
    struct strict_digraph *gr,
    size_t node_ct_size,
    size_t edge_ct_size,
    size_t nedges,
    const struct graph_el_manage *el_mtds
) {
    struct el_manage mng;

    mng = (struct el_manage){
        .destroy = destroy_node,
        .super = gr,
    };

    gr->node_ct_size = node_ct_size;
    gr->edge_ct_size = edge_ct_size;
    gr->nedges = nedges;
    gr->el_mtds = *el_mtds;
    gr->cur_node_id = 0;
    vector_init(VEC(&gr->nodes), sizeof(void *), &mng);
}

void strict_digraph_free(struct strict_digraph *g) {
    if (!g) return;
    vector_free(VEC(&g->nodes));
}

void strict_digraph_reid_nodes(struct strict_digraph *g) {
    for (int i = 0; i < VECTOR_SIZE(&g->nodes); ++i) {
        void *node = *VECTOR_ACCESS(&g->nodes, i);
        *NODE_ID_PTR(g, node) = i;
    }
    g->cur_node_id = VECTOR_SIZE(&g->nodes);
}

enum GR_STATUS strict_digraph_add_node(
    struct strict_digraph *g, const void *node_ct, void **node
) {
    size_t nnodes = VECTOR_SIZE(&g->nodes);
    void **node_ptr;
    *node = NULL;

    if (vector_alloc_back(VEC(&g->nodes), 1) != VEC_ok) return GR_memory_error;
    node_ptr = VECTOR_ACCESS(&g->nodes, nnodes);
    *node_ptr = new_node(g, node_ct);
    if (!node_ptr) return GR_memory_error;

    *node = *node_ptr;
    return GR_ok;
}

enum GR_STATUS
strict_digraph_remove_node(struct strict_digraph *g, void *node_ptr) {
    size_t id_to_remove = VECTOR_SIZE(&g->nodes);

    for (int i = 0; i < VECTOR_SIZE(&g->nodes); ++i) {
        void *cur = *VECTOR_ACCESS(&g->nodes, i);
        if (cur == node_ptr) id_to_remove = i;

        for (int j = 0; j < g->nedges; ++j) {
            if (IS_EMPTY_EDGE(g, cur, j)) continue;
            if (*NODE_EDGE_TO_PTR(g, cur, j) == node_ptr)
                *NODE_EDGE_TO_PTR(g, cur, j) = NULL;
        }
    }

    if (vector_remove(VEC(&g->nodes), id_to_remove) != VEC_ok)
        return GR_key_error;
    return GR_ok;
}

enum GR_STATUS strict_digraph_link_nodes(
    struct strict_digraph *g,
    void *from_node,
    void *to_node,
    const void *edge_ct
) {
    size_t new_edge_id;

    new_edge_id = find_empty_edge_id(g, from_node);
    if (new_edge_id == g->nedges) return GR_edge_error;

    *NODE_EDGE_TO_PTR(g, from_node, new_edge_id) = to_node;
    memcpy(
        NODE_EDGE_CT_PTR(g, from_node, new_edge_id), edge_ct, g->edge_ct_size
    );

    return GR_ok;
}

enum GR_STATUS strict_digraph_unlink_nodes(
    struct strict_digraph *g, void *from_node, void *to_node
) {
    size_t nnodes = VECTOR_SIZE(&g->nodes);
    size_t edge_id;

    edge_id = find_edge_to(g, from_node, to_node);
    if (edge_id == g->nedges) return GR_edge_error;

    *NODE_EDGE_TO_PTR(g, from_node, edge_id) = NULL;
    DESTROY_EDGE_CT(g, from_node, edge_id);

    return GR_ok;
}
