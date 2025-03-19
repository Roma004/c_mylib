#pragma once

#define min(a, b) ((a) < (b) ? a : b)
#define max(a, b) ((a) > b ? a : b)

#define swap(a, b, typename) \
    {                        \
        typename tmp = a;    \
        a = b;               \
        b = tmp;             \
    }

typedef int (*compar_t)(void *a, void *b);

#define DIV_CEIL(a, b) ((a) + (b) - 1) / (b)

#define MAYBE_UNUSED __attribute__((unused))

#define PAIR(type1, type2) \
    struct {               \
        type1 first;       \
        type2 second;      \
    }
