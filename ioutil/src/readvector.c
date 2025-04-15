#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ioutil/readline.h"
#include "ioutil/readvector.h"
#include "vector/vector.h"

int conv_str_to_long(const char *src, void *dst) {
    char *end = NULL;
    long res = strtol(src, &end, 10);
    if (res == LONG_MIN || res == LONG_MAX || end == src
        || end != src + strlen(src)) {
        return 0;
    }
    memcpy(dst, &res, sizeof(long));
    return 1;
}

int conv_str_to_int(const char *src, void *dst) {
    char *end = NULL;
    long lres;
    if (!conv_str_to_long(src, &lres)) return 0;
    if (lres < INT_MIN || lres > INT_MAX) return 0;
    int res = (int)lres;
    memcpy(dst, &res, sizeof(int));
    return 1;
}

int read_objects(const char *prompt, struct vector *vec, conv_t conv) {
    char *buf = NULL;
    void *tmp = malloc(VECTOR_EL_SIZE(vec));
    int restart_flag = 0;
    size_t buf_len;
    char *word;
    size_t idx;

    while (!restart_flag) {
        vector_clear(vec);
        free(buf);
        buf = NULL;

        if ((buf = readline(prompt)) == NULL) goto error;
        buf_len = strlen(buf);

        word = strtok(buf, " \t");
        while (word != NULL) {
            if (!conv(word, tmp)) {
                fprintf(stderr, "Invalid value passed!\n");
                restart_flag = 1;
                break;
            }
            if (vector_push_back(vec, tmp) != VEC_ok) goto error;
            idx = word - buf + strlen(word) + 1;
            if (idx >= buf_len) break;
            word = strtok(buf + idx, " \t");
        }
    }

    free(buf);
    free(tmp);
    return 1;

error:
    vector_free(vec);
    free(buf);
    free(tmp);
    return 0;
}

int read_vector(
    const char *prompt, void *dst, size_t size, size_t len, conv_t conv
) {
    char *buf = NULL;
    while (1) {
        free(buf);
        buf = readline(prompt);
        if (buf == NULL) return 0;

        char *word = strtok(buf, " \t");
        size_t idx = 0;
        int restart_flag = 0;
        while (word != NULL) {
            if (idx < len && !conv(word, dst + idx * size)) {
                fprintf(stderr, "Invalid value passed!\n");
                restart_flag = 1;
                break;
            }
            idx += 1;
            word = strtok(NULL, " \t");
        }
        if (restart_flag) continue;
        if (idx != len) {
            fprintf(
                stderr,
                "Sequence of length `%lu` expected (`%lu` passed)!\n",
                len,
                idx
            );
            continue;
        }
        break;
    }
    free(buf);

    return 1;
}

int read_vector_long(const char *prompt, long *dst, size_t len) {
    return read_vector(prompt, dst, sizeof(long), len, conv_str_to_long);
}
int read_vector_int(const char *prompt, int *dst, size_t len) {
    return read_vector(prompt, dst, sizeof(int), len, conv_str_to_int);
}
