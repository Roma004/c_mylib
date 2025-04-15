#pragma once

struct el_manage {
    const void *super;
    // optional:
    void (*destroy)(const void *super, void *self);
    void (*default_init)(const void *super, void *self);
};

enum VEC_STATUS { VEC_ok = 0, VEC_memory_error, VEC_index_error };
