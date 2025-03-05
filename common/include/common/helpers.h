#pragma once
#include <stddef.h>
/////////////////////////////// HASH FUNCTIONS /////////////////////////////////

#define DEFINE_HASH_FUNC(type_name) size_t type_name##_hash(const void *ptr)

#define DECLARE_HASH_FUNC(type_name, nbytes)      \
    DEFINE_HASH_FUNC(type_name) {                 \
        size_t res = 0x7f7f7f7f7f7f7f7f;          \
        for (int i = 0; i < nbytes; ++i) {        \
            res = 37 * res + *((uint8_t *)ptr++); \
        }                                         \
        return res;                               \
    }

DEFINE_HASH_FUNC(cstr_ptr);
DEFINE_HASH_FUNC(uint32);
DEFINE_HASH_FUNC(uint64);
DEFINE_HASH_FUNC(uint);

///////////////////////////////// COMPARATORS /////////////////////////////////
#define DEFINE_COMPARATOR(type_name) \
    int type_name##_cmp(const void *a, const void *b)

#define DECLARE_COMPARATOR(type_name, nbytes) \
    DEFINE_COMPARATOR(type_name) { return memcmp(a, b, nbytes) == 0; }

#define DECLARE_POINTER_COMPARATOR(type_name, ptr_type)                   \
    DEFINE_COMPARATOR(type_name)(const void *a, const void *b) {          \
        return memcmp(*(ptr_type **)a, *(ptr_type **)b, sizeof(ptr_type)) \
            == 0;                                                         \
    }

DEFINE_COMPARATOR(cstr_ptr);
DEFINE_COMPARATOR(uint32);
DEFINE_COMPARATOR(uint64);
DEFINE_COMPARATOR(uint);

///////////////////////////////// DESCTUCTORS //////////////////////////////////

#define DECLARE_PAIR_DESTRUCTOR_HANDLE_FIRST(name, type1, type2, free_mtd) \
    void name(void *ptr) { free_mtd(((PAIR(type1, type2) *)ptr)->first); }

#define DECLARE_PAIR_DESTRUCTOR_HANDLE_SECOND(name, type1, type2, free_mtd) \
    void name(void *ptr) { free_mtd(((PAIR(type1, type2) *)ptr)->second); }

#define DECLARE_PAIR_DESTRUCTOR_HANDLE_BOTH(            \
    name, type1, type2, free_mtd1, free_mtd2            \
)                                                       \
    void name(void *ptr) {                              \
        free_mtd1(((PAIR(type1, type2) *)ptr)->first);  \
        free_mtd2(((PAIR(type1, type2) *)ptr)->second); \
    }

#define DECLARE_DESTRUCTOR(name, type, free_mtd) \
    void name(void *ptr) { free_mtd(*(type *)ptr); }

/////////////////////////////// DEFAUL CONSTRUCTORS ///////////////////////////

#define DEFINE_ZERO_DEFAULT_INIT(type_name) \
    void type_name##_default_init(void *ptr)

#define DECLARE_ZERO_DEFAULT_INIT(type_name, type) \
    DEFINE_ZERO_DEFAULT_INIT(type_name) { memset(ptr, 0, sizeof(type)); }

DEFINE_ZERO_DEFAULT_INIT(uint64);
DEFINE_ZERO_DEFAULT_INIT(uint32);
DEFINE_ZERO_DEFAULT_INIT(uint);
DEFINE_ZERO_DEFAULT_INIT(float);
DEFINE_ZERO_DEFAULT_INIT(double);
DEFINE_ZERO_DEFAULT_INIT(pointer);
