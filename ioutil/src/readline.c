#define _XOPEN_SOURCE
#include <ioutil/readline.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *freadline(FILE *ifd) {
    char *res = NULL;
    size_t len = 1;
    char buf[81] = {};

    int scan_res = 0;
    while ((scan_res = fscanf(ifd, "%80[^\n]s", buf)) != EOF && scan_res != 0) {
        len += 80;
        res = (char *)realloc(res, len * sizeof(char *));
        if (res == NULL) return NULL;
        strncpy(res + len - 81, buf, 80);
    }
    if (scan_res == 0 && res == NULL) res = (char *)calloc(1, sizeof(char));
    fgetc(ifd);
    return res;
}

char *readline(const char *prompt) {
    printf("%s", prompt);
    return freadline(stdin);
}

char *read_filename(const char *default_filename) {
    char *res = NULL;
    char *prompt = "Enter filename: ";
    int use_default = 0;
    if (default_filename) {
        const char *format = "Enter filename (default=%s): ";
        use_default = 1;
        prompt = (char *)malloc(
            (strlen(format) + strlen(default_filename) + 1) * sizeof(char));
        if (prompt == NULL) goto error;
        sprintf(prompt, format, default_filename);
    }

    char *filename;
    while (1) {
        filename = readline(prompt);
        if (filename == NULL) goto error;
        if (*filename == 0) {
            free(filename);
            if (use_default) filename = (char *)default_filename;
            else continue;
        }
        res = filename;
        break;
    }

error:
    if (use_default) free(prompt);
    return res;
}

int read_date(const char *prompt, const char *format, struct tm *time) {
    while (1) {
        char *inp = readline(prompt);
        if (inp == NULL) return 1;
        if (strptime(inp, format, time) == NULL) {
            printf("Invalid date format!\n");
            free(inp);
            continue;
        }
        break;
        free(inp);
    }

    return 1;
}
