#include "os_cli.h"
#include "global_includes.h"

#define MAX_COMMANDS_LIST 16

static int num_cmd = 0;
static main_command_cb_cli_t *commands_list[MAX_COMMANDS_LIST];
static os_uart_t cli_uart;
int add_command_command_list(main_command_cb_cli_t *main_cmd){
    if(num_cmd == MAX_COMMANDS_LIST-1){
        return OS_RET_NO_AVAILABLE_DATA;
    }

    // Add to pointer list
    commands_list[num_cmd] = main_cmd;

    return OS_RET_OK;
}

void cli_init(void *parameters){
    int ret = os_uart_begin(&cli_uart, 0, 115200);
    HAL_ASSERT(ret != OS_RET_OK);
}

void cli_task(void *parameters){
    char in_buff[512];
    for(;;){
        size_t size = os_uart_readstring(&cli_uart, (uint8_t*)in_buff, sizeof(in_buff));

        os_thread_sleep_ms(10);
    }
}
