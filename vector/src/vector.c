#include "vector/vector.h"
#include "common/common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// struct vector {
//     __VECTOR_FIELDS(void)
// };

static inline enum VEC_STATUS
vector_data_realloc(struct vector *vec, size_t new_size) {
    if (new_size == 0) {
        vector_clear(vec);
        return VEC_ok;
    }
    if (new_size < vec->size && vec->el_mtds.destroy) {
        for (int i = new_size; i < vec->size; ++i)
            vec->el_mtds.destroy(VECTOR_ACCESS(vec, i));
    }
    void *tmp = realloc(vec->data, new_size * vec->el_size);
    if (!tmp) return VEC_memory_error;

    vec->data = tmp;
    vec->capacity = new_size;
    return VEC_ok;
}

static inline enum VEC_STATUS vector_expand(struct vector *vec) {
    size_t new_capacity = vec->capacity;
    if (new_capacity == 0) new_capacity = 2;
    if (new_capacity <= (1 << 10)) new_capacity *= 2;
    else if (new_capacity <= (1 << 15)) new_capacity += (1 << 10);
    else if (new_capacity <= (1 << 20)) new_capacity += (1 << 15);
    else new_capacity += (1 << 20);
    return vector_data_realloc(vec, new_capacity);
}

static inline enum VEC_STATUS vector_shrink(struct vector *vec) {
    size_t new_capacity = vec->capacity;
    new_capacity /= 2;
    return vector_data_realloc(vec, new_capacity);
}

struct vector *vector_new(size_t el_size, const struct el_manage *mtds) {
    struct vector *res = malloc(sizeof(*res));
    if (res == NULL) return res;
    vector_init(res, el_size, mtds);
    return res;
}

void vector_init(
    struct vector *vec, size_t el_size, const struct el_manage *mtds
) {
    *vec = (struct vector){.capacity = 0,
                           .size = 0,
                           .data = NULL,
                           .el_size = el_size,
                           .el_mtds = (struct el_manage){}};
    if (mtds) vec->el_mtds = *mtds;
}

enum VEC_STATUS vector_push_back(struct vector *vec, const void *el) {
    if (vec->size + 1 >= vec->capacity) {
        if (vector_expand(vec) != VEC_ok) return VEC_memory_error;
    }
    memcpy(VECTOR_ACCESS(vec, vec->size++), el, vec->el_size);
    return VEC_ok;
}

enum VEC_STATUS vector_insert(struct vector *vec, const void *el, size_t idx) {
    if (idx > vec->size) return VEC_index_error;
    if (vec->size + 1 >= vec->capacity) {
        if (vector_expand(vec) != VEC_ok) return VEC_memory_error;
    }
    if (idx != vec->size)
        memmove(
            vec->data + (idx + 1) * vec->el_size,
            vec->data + idx * vec->el_size,
            (vec->size - idx) * vec->el_size
        );
    memcpy(vec->data + idx * vec->el_size, el, vec->el_size);
    vec->size += 1;
    return VEC_ok;
}

void vector_clear(struct vector *vec) {
    if (vec == NULL) return;
    if (vec->el_mtds.destroy) {
        for (int i = 0; i < vec->size; ++i) {
            vec->el_mtds.destroy(VECTOR_ACCESS(vec, i));
        }
    }
    free(vec->data);
    vec->data = NULL;
    vec->capacity = 0;
    vec->size = 0;
}

enum VEC_STATUS vector_resize(struct vector *vec, size_t new_size) {
    enum VEC_STATUS stt = vector_data_realloc(vec, new_size);
    if (stt != VEC_ok) return stt;
    vec->size = min(new_size, vec->size);
    return VEC_ok;
}

enum VEC_STATUS vector_alloc_back(struct vector *vec, size_t els_num) {
    enum VEC_STATUS stt;
    if (vec->capacity < vec->size + els_num) {
        if (els_num == 1) vector_expand(vec);
        else if ((stt = vector_data_realloc(vec, vec->size + els_num))
                 != VEC_ok)
            return stt;
    }
    vec->size += els_num;
    if (vec->el_mtds.default_init) {
        for (int i = 0; i < els_num; ++i) {
            vec->el_mtds.default_init(VECTOR_ACCESS_RIHGT(vec, i));
        }
    }
    return VEC_ok;
}

void vector_free(struct vector *vec) { vector_clear(vec); }

void vector_delete(struct vector *vec) {
    if (vec == NULL) return;
    vector_free(vec);
    free(vec);
}

enum VEC_STATUS vector_get(const struct vector *vec, void *dst, size_t idx) {
    if (idx >= vec->size) return VEC_index_error;
    memcpy(dst, VECTOR_ACCESS(vec, idx), vec->el_size);
    return VEC_ok;
}

enum VEC_STATUS vector_set(struct vector *vec, const void *src, size_t idx) {
    if (idx >= vec->size) return VEC_index_error;
    memcpy(VECTOR_ACCESS(vec, idx), src, vec->el_size);
    return VEC_ok;
}

enum VEC_STATUS vector_remove(struct vector *vec, size_t idx) {
    if (idx >= vec->size) return VEC_index_error;
    if (vec->el_mtds.destroy) { vec->el_mtds.destroy(VECTOR_ACCESS(vec, idx)); }
    if (idx != vec->size - 1)
        memmove(
            VECTOR_ACCESS(vec, idx),
            VECTOR_ACCESS(vec, idx + 1),
            (vec->size - idx - 1) * vec->el_size
        );
    vec->size -= 1;
    return VEC_ok;
}
