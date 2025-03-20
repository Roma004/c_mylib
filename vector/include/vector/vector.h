#pragma once

#include "common.h"
#include <stddef.h>
#include <stdint.h>

#define __VECTOR_FIELDS(type) \
    type *data;               \
    size_t el_size;           \
    size_t size;              \
    size_t capacity;          \
    struct el_manage el_mtds;

#define Vector(type)          \
    struct {                  \
        __VECTOR_FIELDS(type) \
    }

struct vector {
    __VECTOR_FIELDS(void);
};

/**
 * @brief Инициализировать структуру вектора
 * @param[vec] указатель на структуру вектора для инициализации
 * @param[el_size] размер хранимого элемента
 * @param[mtds] методы управления элементами вектора
 * */
void vector_init(
    struct vector *vec, size_t el_size, const struct el_manage *mtds
);

/**
 * @brief добавить элемент в конец вектора.
 * Значение по переданному указателю будет скопировано в конец
 */
enum VEC_STATUS vector_push_back(struct vector *vec, const void *el);

/**
 * @brief изменить размер массива.
 *
 * После этой опреации будет выделено памяти ровно под new_size элементов
 * Количество реалино хранимых элементов может измениться только в меньшую
 * сторону.
 */
enum VEC_STATUS vector_resize(struct vector *vec, size_t new_size);

/**
 * @brief Добавить пространство для заданного количества элементов в конец
 *
 * - После этой опреации размер (size) вектора увеличится на els_num.
 * - Размер выделенной памяти (capacity) будет достаточен для хранения
 *   size + els_num элементов.
 * - Обращение по индексам диапазона [size, size+els_num) станет безопасным
 *   для записи, но чтение от туда - UB.
 * - если определён метод инициализации по-умолчанию, он будет вызван в новом
 *   пространстве
 */
enum VEC_STATUS vector_alloc_back(struct vector *vec, size_t els_num);

enum VEC_STATUS vector_remove(struct vector *vec, size_t idx);
enum VEC_STATUS vector_insert(struct vector *vec, const void *el, size_t idx);

enum VEC_STATUS vector_get(const struct vector *vec, void *dst, size_t idx);
enum VEC_STATUS vector_set(struct vector *vec, const void *src, size_t idx);

void vector_clear(struct vector *vec);
void vector_free(struct vector *vec);

#define VECTOR_SIZE(vec)    ((vec)->size)
#define VECTOR_EL_SIZE(vec) ((vec)->el_size)

#define VEC(vec) ((struct vector *)(vec))

#define VECTOR_INIT(vec, _default_init, _destroy, _super)                       \
    {                                                                           \
        void (*init_fn)(const typeof(*_super) *, typeof(*((vec)->data)) *) =    \
            _default_init;                                                      \
        void (*destroy_fn)(const typeof(*_super) *, typeof(*((vec)->data)) *) = \
            _destroy;                                                           \
        struct el_manage __mtds = {                                             \
            .default_init = (void (*)(const void *, void *))init_fn,            \
            .destroy = (void (*)(const void *, void *))destroy_fn,              \
            .super = _super,                                                    \
        };                                                                      \
        vector_init((void *)(vec), sizeof(*((vec)->data)), &__mtds);            \
    }

#define VECTOR_PUSH_BACK(status, vec, _el)        \
    {                                             \
        typeof((vec)->data[0]) el = _el;          \
        status = vector_push_back(VEC(vec), &el); \
    }

#define VECTOR_ACCESS(vec, idx) \
    ((typeof(*(vec)->data) *)((uint8_t *)(vec)->data + (idx) * (vec)->el_size))

#define VECTOR_ACCESS_RIHGT(vec, idx)                            \
    ((typeof(*((vec)->data)) *)((uint8_t *)(vec)->data           \
                                + (VECTOR_SIZE(vec) - (idx) - 1) \
                                      * (vec)->el_size))

#define VECTOR_FREE(vec) vector_free(VEC(vec))

#define VECTOR_CLEAR(vec) vector_clear(VEC(vec))
