#pragma once

enum GR_STATUS {
    GR_ok = 0,
    GR_memory_error,
    GR_key_error,
    GR_edge_error,
};

struct graph_el_manage {
    const void *super;

    // optional:
    void (*node_ct_destroy)(const void *super, void *self);
    void (*edge_ct_destroy)(const void *super, void *self);
};

struct tree_el_manage {
    const void *super;

    int (*key_cmp_eq)(const void *super, const void *self, const void *other);
    // optional:
    void (*key_destroy)(const void *super, void *self);
    void (*val_destroy)(const void *super, void *self);
};
