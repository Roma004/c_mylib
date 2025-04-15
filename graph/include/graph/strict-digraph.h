#pragma once
#include "common.h"
#include "vector/vector.h"
#include <stddef.h>
#include <stdint.h>

#define StrictDigraphEdge(edge_ct_type) \
    struct {                            \
        void *to;                       \
        edge_ct_type ct;                \
    } __attribute__((packed))

#define StrictDigraphNode(node_ct_type, edge_ct_type, nedges) \
    struct {                                                  \
        size_t node_id;                                       \
        StrictDigraphEdge(edge_ct_type) edges[nedges];        \
        node_ct_type ct;                                      \
    } __attribute__((packed))

#define __STRICT_DIGRAPH_BASICS \
    size_t node_ct_size;        \
    size_t edge_ct_size;        \
    size_t nedges;              \
    size_t cur_node_id;         \
    struct graph_el_manage el_mtds;

#define StrictDigraph(node_ct_type, edge_ct_type, _nedges)                    \
    struct {                                                                  \
        Vector(StrictDigraphNode(node_ct_type, edge_ct_type, _nedges)) nodes; \
        __STRICT_DIGRAPH_BASICS                                               \
    }

#define NamedStrictDigraphEdge(_typename, edge_ct_type) \
    struct _typename##_gedge {                          \
        struct _typename##_gnode *to;                   \
        edge_ct_type ct;                                \
    } __attribute__((packed))

#define NamedStrictDigraphNode(_typename, node_ct_type, nedges) \
    struct _typename##_gnode {                                  \
        size_t node_id;                                         \
        struct _typename##_gedge edges[nedges];                 \
        node_ct_type ct;                                        \
    } __attribute__((packed))

#define NamedStrictDigraph(_typename)      \
    struct _typename {                     \
        Vector(_typename##_gnode *) nodes; \
        __STRICT_DIGRAPH_BASICS            \
    }

#define DECLARE_STRICT_DIGRAPH(_typename, node_ct_type, edge_ct_type, _nedges) \
    typedef NamedStrictDigraphEdge(_typename, edge_ct_type) _typename##_gedge; \
    typedef NamedStrictDigraphNode(_typename, node_ct_type, _nedges)           \
        _typename##_gnode;                                                     \
    typedef NamedStrictDigraph(_typename) _typename;

struct strict_digraph {
    Vector(void *) nodes;
    __STRICT_DIGRAPH_BASICS;
};

void strict_digraph_init(
    struct strict_digraph *gr,
    size_t node_ct_size,
    size_t edge_ct_size,
    size_t nedges,
    const struct graph_el_manage *el_mtds
);

void strict_digraph_free(struct strict_digraph *g);

enum GR_STATUS strict_digraph_add_node(
    struct strict_digraph *g, const void *node_ct, void **node
);

enum GR_STATUS
strict_digraph_remove_node(struct strict_digraph *g, void *node_ptr);

enum GR_STATUS strict_digraph_link_nodes(
    struct strict_digraph *g,
    void *from_node,
    void *to_node,
    const void *edge_ct
);

enum GR_STATUS strict_digraph_unlink_nodes(
    struct strict_digraph *g, void *from_node, void *to_node
);

void strict_digraph_reid_nodes(struct strict_digraph *g);

#define SDGR(gr) (struct strict_digraph *)(gr)

#define SDGRAPH_NODES_NUM(gr) (VECTOR_SIZE(&(gr)->nodes))
#define SDGRAPH_EDGES_NUM(gr) ((gr)->nedges)

#define SDGRAPH_NODE(gr, node_id) ((gr)->nodes.data[node_id])

#define __SDGRAPH_TYPEOF_NODE(gr)    typeof((**(gr)->nodes.data))
#define __SDGRAPH_TYPEOF_NODE_CT(gr) typeof((*(gr)->nodes.data)->ct)
#define __SDGRAPH_TYPEOF_EDGE_CT(gr) typeof((*(gr)->nodes.data)->edges[0].ct)
#define __SDGRAPH_SIZEOF_NODE_CT(gr) sizeof((*(gr)->nodes.data)->ct)
#define __SDGRAPH_SIZEOF_EDGE_CT(gr) sizeof((*(gr)->nodes.data)->edges[0].ct)
#define __SDGRAPH_SIZEOF_EDGES(gr)   sizeof((*(gr)->nodes.data)->edges)
#define __SDGRAPH_SIZEOF_EDGE(gr)    sizeof((*(gr)->nodes.data)->edges[0])

#define SDGRAPH_EDGE(gr, node_id, edge_id) \
    (SDGRAPH_NODE_EDGES(gr, node_id)[edge_id])

#define SDGRAPH_EDGE_CT(gr, node_id, edge_id) \
    (SDGRAPH_EDGE(gr, node_id, edge_id).ct)

#define SDGRAPH_EDGE_CONNECTED_NODE(gr, node_id, edge_id) \
    (typeof(SDGRAPH_NODE(gr, 0)) *)(SDGRAPH_EDGE(gr, node_id, edge_id).to)

#define SDGRAPH_EDGE_CONNECTED_ID(gr, node_id, edge_id) \
    SDGRAPH_NODE_ID(gr, SDGRAPH_EDGE_CONNECTED_NODE(gr, node_id, edge_id))

#define SDGRAPH_EDGE_CONNECTED_CT(gr, node_id, edge_id) \
    SDGRAPH_NODE_CT(gr, SDGRAPH_EDGE_CONNECTED_ID(gr, node_id, edge_id))

#define SDGRAPH_INIT(gr, _node_ct_destroy, _edge_ct_destroy, _super)            \
    {                                                                           \
        void (*destroy_node_fn)(const void *, __SDGRAPH_TYPEOF_NODE_CT(gr) *) = \
            _node_ct_destroy;                                                   \
        void (*destroy_edge_fn)(const void *, __SDGRAPH_TYPEOF_EDGE_CT(gr) *) = \
            _edge_ct_destroy;                                                   \
        struct graph_el_manage __mtds = {                                       \
            .node_ct_destroy =                                                  \
                (void (*)(const void *, void *))destroy_node_fn,                \
            .edge_ct_destroy =                                                  \
                (void (*)(const void *, void *))destroy_edge_fn,                \
            .super = _super,                                                    \
        };                                                                      \
        strict_digraph_init(                                                    \
            SDGR(gr),                                                           \
            __SDGRAPH_SIZEOF_NODE_CT(gr),                                       \
            __SDGRAPH_SIZEOF_EDGE_CT(gr),                                       \
            __SDGRAPH_SIZEOF_EDGES(gr) / __SDGRAPH_SIZEOF_EDGE(gr),             \
            &__mtds                                                             \
        );                                                                      \
    }

#define SDGRAPH_ADD_NODE(status, gr, content, node_ptr)                        \
    {                                                                          \
        __SDGRAPH_TYPEOF_NODE_CT(gr) node_ct = content;                        \
        __SDGRAPH_TYPEOF_NODE(gr) * __ptr;                                     \
        status = strict_digraph_add_node(SDGR(gr), &node_ct, (void **)&__ptr); \
        node_ptr = __ptr;                                                      \
    }

#define SDGRAPH_REMOVE_NODE(status, gr, node_ptr)             \
    {                                                         \
        __SDGRAPH_TYPEOF_NODE(gr) *__ptr = node_ptr;          \
        status = strict_digraph_remove_node(SDGR(gr), __ptr); \
    }

#define SDGRAPH_LINK_NODES(status, gr, from, to, content)                 \
    {                                                                     \
        __SDGRAPH_TYPEOF_EDGE_CT(gr) edge_ct = content;                   \
        status = strict_digraph_link_nodes(SDGR(gr), from, to, &edge_ct); \
    }

#define SDGRAPH_UNLINK_NODES(status, gr, from, to) \
    { status = strict_digraph_unlink_nodes(SDGR(gr), from, to); }

#define SDGRAPH_FREE(gr) strict_digraph_free(SDGR(gr));

#define SDGRAPH_REID_NODES(gr) strict_digraph_reid_nodes(SDGR(gr));
