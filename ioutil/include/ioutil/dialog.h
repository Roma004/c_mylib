#include "scan.h"

static inline int dialog(const char *prompt, const char *choices, char *c) {
    while (1) {
        if (scan_char(prompt, c) == 0) {
            return 0;
        }
        for (char *ch = (char *)choices; *ch; ch++) {
            if (*c == *ch) return 1;
        }
        continue;
    }
}

static inline int dialog_yn(const char *prompt, char *c) {
    return dialog(prompt, "yn", c);
}
