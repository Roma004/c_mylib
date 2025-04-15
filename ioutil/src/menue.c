#include "ioutil/menue.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ioutil/scan.h"

static void print_help(struct menue_conf *conf) {
    puts("\nFunctionality available:\n");
    for (int i = 0; i < conf->num_entries; ++i) {
        printf(
            "  %c -- %s\n", conf->entries[i].cmd, conf->entries[i].description
        );
    }
    puts("\nSpecial mappings:\n");
    printf(
        "  %c -- print this menue\n"
        "  %c -- quite program\n",
        conf->menue_key,
        conf->quite_key
    );
}

menue_handler_status menue_loop(struct menue_conf *conf) {
    char *cmds = (char *)malloc(96 * sizeof(char));
    memset(cmds, 0, 96 * sizeof(char));
    for (int i = 0; i < conf->num_entries; ++i) {
        cmds[conf->entries[i].cmd - 32] = (char)(i + 1);
    }

    conf->hello_msg_handler();
    int quit_status = 0;
    menue_handler_status status = MENUE_quit;
    while (!quit_status) {
        char cmd;
        int scan_res = scan_char(conf->prompt, &cmd);
        if (cmd < 32) continue;
        if (!scan_res) break;
        if (cmd == conf->quite_key) break;
        if (cmd == conf->menue_key) {
            print_help(conf);
            continue;
        }
        if (cmds[cmd - 32] == 0) {
            puts("Unknown command! Please, try again.");
            continue;
        }
        struct menue_entry_conf *handler = &conf->entries[cmds[cmd - 32] - 1];
        menue_handler_status handler_res =
            handler->handler(handler->handler_data);
        switch (handler_res) {
        case MENUE_ok: break;
        case MENUE_op_interrupted: break;
        case MENUE_quit:
        case MENUE_error:
            quit_status = 1;
            status = handler_res;
            break;
        }
    }

    free(cmds);
    return status;
}
