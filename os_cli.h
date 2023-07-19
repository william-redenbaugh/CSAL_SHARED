#ifndef _OS_CLI_H
#define _OS_CLI_H

#include "stdlib.h"

typedef void (*cli_cmd_cb_t)(char *params, size_t size);

typedef struct sub_command_cb_cli{
    const char *cmd;
    const char *help;
    cli_cmd_cb_t cli_func;
}sub_command_cb_cli_t;

typedef struct main_command_cb_cli{
    const char *cmd;
    sub_command_cb_cli_t *cli_list;
    size_t cli_list_len;
}main_command_cb_cli_t;

int add_command_command_list(main_command_cb_cli_t *main_cmd);
#endif