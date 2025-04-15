#pragma once

#include "vector/vector.h"
#include <stddef.h>

typedef int (*conv_t)(const char *src, void *dst);

int conv_str_to_long(const char *src, void *dst);

int read_vector(
    const char *prompt, void *dst, size_t size, size_t len, conv_t conv
);
int read_vector_long(const char *prompt, long *dst, size_t len);
int read_vector_int(const char *prompt, int *dst, size_t len);

int read_objects(const char *prompt, struct vector *vec, conv_t conv);
