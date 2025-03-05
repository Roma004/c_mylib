#pragma once

#include <stddef.h>

typedef enum menue_handler_status {
    MENUE_ok = 0,
    MENUE_op_interrupted,
    MENUE_error,
    MENUE_quit
} menue_handler_status;

typedef menue_handler_status (*menue_handler_t)(void *);

struct menue_entry_conf {
    const char cmd;
    const char *description;
    menue_handler_t handler;
    void *handler_data;
};

struct menue_conf {
    struct menue_entry_conf *entries;
    size_t num_entries;
    const char *prompt;
    void (*hello_msg_handler)(void);
    char quite_key;
    char menue_key;
};

menue_handler_status menue_loop(struct menue_conf *conf);
