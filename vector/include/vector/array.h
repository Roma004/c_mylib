#pragma once

#include "common.h"
#include <stddef.h>
#include <stdint.h>

#define __ARRAY_FIELDS(type) \
    type *data;              \
    size_t el_size;          \
    size_t size;             \
    struct el_manage el_mtds;

#define Array(type)          \
    struct {                 \
        __ARRAY_FIELDS(type) \
    }

struct array {
    __ARRAY_FIELDS(void);
};

/**
 * @brief Инициализировать структуру массива
 * @param[arr] указатель на структуру массива для инициализации
 * @param[el_size] размер хранимого элемента
 * @param[mtds] методы управления элементами массива
 * */
void array_init(
    struct array *arr, size_t el_size, const struct el_manage *mtds
);

/**
 * @brief изменить размер массива.
 *
 * После этой опреации будет выделено памяти ровно под new_size элементов.
 * Новые элементы будут инициализированы значениями по-умолчанию
 */
enum VEC_STATUS array_resize(struct array *arr, size_t new_size);

enum VEC_STATUS array_get(const struct array *arr, void *dst, size_t idx);
enum VEC_STATUS array_set(struct array *arr, const void *src, size_t idx);

void array_clear(struct array *arr);
void array_free(struct array *arr);

#define ARRAY_SIZE(arr)    ((arr)->size)
#define ARRAY_EL_SIZE(arr) ((arr)->el_size)

#define ARR(arr) ((struct array *)(arr))

#define ARRAY_INIT(arr, _default_init, _destroy, _super)                        \
    {                                                                           \
        void (*init_fn)(const typeof(*_super) *, typeof(*((arr)->data)) *) =    \
            _default_init;                                                      \
        void (*destroy_fn)(const typeof(*_super) *, typeof(*((arr)->data)) *) = \
            _destroy;                                                           \
        struct el_manage __mtds = {                                             \
            .default_init = (void (*)(const void *, void *))init_fn,            \
            .destroy = (void (*)(const void *, void *))destroy_fn,              \
            .super = _super,                                                    \
        };                                                                      \
        array_init((void *)(arr), sizeof(*((arr)->data)), &__mtds);             \
    }

#define ARRAY_ACCESS(arr, idx)                         \
    ((typeof(*((arr)->data)) *)((uint8_t *)(arr)->data \
                                + (idx) * (arr)->el_size))

#define ARRAY_ACCESS_RIHGT(arr, idx)                            \
    ((typeof(*((arr)->data)) *)((uint8_t *)(arr)->data          \
                                + (ARRAY_SIZE(arr) - (idx) - 1) \
                                      * (arr)->el_size))

#define ARRAY_FREE(arr) array_free(ARR(arr))

