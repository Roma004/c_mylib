#include "ioutil/scan.h"

#include "stdio.h"

typedef int (*scan_func_t)(FILE *, void *, int *);

static int scan_check(FILE *ifd, void *res, scan_func_t sf) {
    int scanf_res = 0;
    int chars_read = 0;

    chars_read = 0;
    scanf_res = sf(ifd, res, &chars_read);

    if (scanf_res == EOF) return EOF;

    fscanf(ifd, "%*[^\n]");
    fgetc(ifd);
    if (!(scanf_res) || (chars_read != 0)) return 0;

    return 1;
}

#define SCAN_CHECK_FN_NAME(type_) type_##_scan_check
#define SCAN_FUNC_FN_NAME(type_)  type_##_scan_func

#define SCAN_FUNC_DECLARE(name, specifier, c_type)                            \
    static inline int SCAN_FUNC_FN_NAME(name)(                                \
        FILE * ifd, void *res, int *chars_read                                \
    ) {                                                                       \
        return fscanf(ifd, specifier "%*[^\n]%n", (c_type *)res, chars_read); \
    }                                                                         \
    static inline int SCAN_CHECK_FN_NAME(name)(FILE * ifd, void *res) {       \
        return scan_check(ifd, res, &SCAN_FUNC_FN_NAME(name));                \
    }                                                                         \
    int fscan_##name(FILE *ifd, const char *prompt, c_type *res) {            \
        while (1) {                                                           \
            printf("%s", prompt);                                             \
            int __scanf_res = SCAN_CHECK_FN_NAME(name)(ifd, res);             \
            if (__scanf_res == EOF) return 0;                                 \
            if (!__scanf_res) {                                               \
                printf("Please, enter correct value!\n\n");                   \
                continue;                                                     \
            }                                                                 \
            break;                                                            \
        }                                                                     \
        return 1;                                                             \
    }                                                                         \
    int ranged_fscan_##name(                                                  \
        FILE *ifd, const char *prompt, c_type *res, c_type min, c_type max    \
    ) {                                                                       \
        int scan_res = 0;                                                     \
        while (1) {                                                           \
            scan_res = fscan_##name(ifd, prompt, res);                        \
            if (!scan_res) return 0;                                          \
            if (*(res) < (min)) {                                             \
                printf("value is too small!\n\n");                            \
                continue;                                                     \
            }                                                                 \
            if (*(res) > (max)) {                                             \
                printf("value is too large!\n\n");                            \
                continue;                                                     \
            }                                                                 \
            break;                                                            \
        }                                                                     \
        return scan_res;                                                      \
    }                                                                         \
    int scan_##name(const char *prompt, c_type *res) {                        \
        return fscan_##name(stdin, prompt, res);                              \
    }                                                                         \
    int ranged_scan_##name(                                                   \
        const char *prompt, c_type *res, c_type min, c_type max               \
    ) {                                                                       \
        return ranged_fscan_##name(stdin, prompt, res, min, max);             \
    }

SCAN_FUNC_DECLARE(int, "%d", int)
SCAN_FUNC_DECLARE(uint, "%lu", unsigned long)
SCAN_FUNC_DECLARE(uint32, "%iu", uint32_t)
SCAN_FUNC_DECLARE(char, "%c", char)
SCAN_FUNC_DECLARE(long_double, "%Lf", long double)
SCAN_FUNC_DECLARE(double, "%lf", double);
SCAN_FUNC_DECLARE(float, "%f", float);

