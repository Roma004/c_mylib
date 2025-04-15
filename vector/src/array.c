#include "vector/array.h"
#include "common/common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline enum VEC_STATUS
array_data_realloc(struct array *arr, size_t new_size) {
    if (new_size == 0) {
        array_clear(arr);
        return VEC_ok;
    }
    if (new_size < arr->size && arr->el_mtds.destroy) {
        for (int i = new_size; i < arr->size; ++i)
            arr->el_mtds.destroy(arr->el_mtds.super, ARRAY_ACCESS(arr, i));
    }
    void *tmp = realloc(arr->data, new_size * arr->el_size);
    if (!tmp) return VEC_memory_error;
    if (new_size > arr->size && arr->el_mtds.default_init) {
        for (int i = arr->size; i < new_size; ++i)
            arr->el_mtds.default_init(arr->el_mtds.super, ARRAY_ACCESS(arr, i));
    }

    arr->data = tmp;
    return VEC_ok;
}

void array_init(
    struct array *arr, size_t el_size, const struct el_manage *mtds
) {
    *arr = (struct array){.size = 0,
                          .data = NULL,
                          .el_size = el_size,
                          .el_mtds = (struct el_manage){}};
    if (mtds) arr->el_mtds = *mtds;
}

void array_clear(struct array *arr) {
    if (arr == NULL) return;
    if (arr->el_mtds.destroy) {
        for (int i = 0; i < arr->size; ++i) {
            arr->el_mtds.destroy(arr->el_mtds.super, ARRAY_ACCESS(arr, i));
        }
    }
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
}

enum VEC_STATUS array_resize(struct array *arr, size_t new_size) {
    enum VEC_STATUS stt = array_data_realloc(arr, new_size);
    if (stt != VEC_ok) return stt;
    arr->size = new_size;
    return VEC_ok;
}

void array_free(struct array *arr) { array_clear(arr); }

enum VEC_STATUS array_get(const struct array *arr, void *dst, size_t idx) {
    if (idx >= arr->size) return VEC_index_error;
    memcpy(dst, ARRAY_ACCESS(arr, idx), arr->el_size);
    return VEC_ok;
}

enum VEC_STATUS array_set(struct array *arr, const void *src, size_t idx) {
    if (idx >= arr->size) return VEC_index_error;
    memcpy(ARRAY_ACCESS(arr, idx), src, arr->el_size);
    return VEC_ok;
}

enum VEC_STATUS array_remove(struct array *arr, size_t idx) {
    if (idx >= arr->size) return VEC_index_error;
    if (arr->el_mtds.destroy) {
        arr->el_mtds.destroy(arr->el_mtds.super, ARRAY_ACCESS(arr, idx));
    }
    if (idx != arr->size - 1)
        memmove(
            ARRAY_ACCESS(arr, idx),
            ARRAY_ACCESS(arr, idx + 1),
            (arr->size - idx - 1) * arr->el_size
        );
    arr->size -= 1;
    return VEC_ok;
}
