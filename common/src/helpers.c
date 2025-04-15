#include "common/helpers.h"
#include <stdint.h>
#include <string.h>

DECLARE_COMPARATOR(uint32, sizeof(uint32_t));
DECLARE_COMPARATOR(uint64, sizeof(uint64_t));
DECLARE_COMPARATOR(uint, sizeof(unsigned int));

DEFINE_COMPARATOR(cstr_ptr) { return strcmp(*(char **)a, *(char **)b) == 0; }

DECLARE_HASH_FUNC(uint32, sizeof(uint32_t));
DECLARE_HASH_FUNC(uint64, sizeof(uint64_t));
DECLARE_HASH_FUNC(uint, sizeof(unsigned int));

DEFINE_HASH_FUNC(cstr_ptr) {
    char *str = *(char **)ptr;
    size_t res = 0x7f7f7f7f7f7f7f7f;
    while (*str) { res = 37 * res + *str++; }
    return res;
}

DECLARE_ZERO_DEFAULT_INIT(uint64, uint64_t);
DECLARE_ZERO_DEFAULT_INIT(uint32, uint32_t);
DECLARE_ZERO_DEFAULT_INIT(uint, unsigned int);
DECLARE_ZERO_DEFAULT_INIT(float, float);
DECLARE_ZERO_DEFAULT_INIT(double, double);
DECLARE_ZERO_DEFAULT_INIT(pointer, void *);
