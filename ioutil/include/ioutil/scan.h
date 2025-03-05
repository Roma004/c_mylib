#pragma once

#include <stdio.h>
#include <stdint.h>

#define SCAN_FUNC_DEFINE(name, c_type)                                     \
    int scan_##name(const char *prompt, c_type *res);                      \
    int ranged_scan_##name(                                                \
        const char *prompt, c_type *res, c_type min, c_type max            \
    );                                                                     \
    int fscan_##name(FILE *ifd, const char *prompt, c_type *res);          \
    int franged_scan_##name(                                               \
        FILE *ifd, const char *prompt, c_type *res, c_type min, c_type max \
    );

SCAN_FUNC_DEFINE(int, int)
SCAN_FUNC_DEFINE(long_double, long double)
SCAN_FUNC_DEFINE(double, double);
SCAN_FUNC_DEFINE(float, float);
SCAN_FUNC_DEFINE(char, char);
SCAN_FUNC_DEFINE(uint, unsigned long)
SCAN_FUNC_DEFINE(uint32, uint32_t)
