#include "os_cli.h"
#include "global_includes.h"

#ifdef OS_CLI_MODULE
#define MAX_COMMANDS_LIST 16

static int num_cmd = 0;
static main_command_cb_cli_t *commands_list[MAX_COMMANDS_LIST];
static os_uart_t cli_uart;
int add_command_command_list(main_command_cb_cli_t *main_cmd)
{
    if (num_cmd == MAX_COMMANDS_LIST - 1)
    {
        return OS_RET_NO_AVAILABLE_DATA;
    }

    // Add to pointer list
    commands_list[num_cmd] = main_cmd;

    return OS_RET_OK;
}

void cli_init(void *parameters)
{
    int ret = os_uart_begin(&cli_uart, 0, 115200);
    HAL_ASSERT(ret != OS_RET_OK);
}

static char *remove_all_front_spaces(char *val)
{
    while (*val == ' ')
        val++;

    return val;
}

static char *find_next_space_separeted_string(char *val){
    // Iterate through  all characters until we find the next space separated command
    while(*val != ' ' && *val != 0){
        val++;
    }

    // If we iterated to a space we increment one more
    if(*val == ' '){
        val++;
    }
    return val;
}

static bool strcmp_cmd(char *cmd, char *str){
    while(*cmd != 0 && (*str != 0 || *str != ' ')){
        
        if(*cmd != *str){
            return false;
        }

        cmd++;
        str++;
    }

    return true;
}

void cli_task(void *parameters)
{
    char in_buff[512];
    for (;;)
    {
        size_t size = os_uart_readstring(&cli_uart, (uint8_t *)in_buff, sizeof(in_buff));

        // Finds point of all spaces
        char *begining_cli = remove_all_front_spaces(in_buff);

        int n;
        for (n = 0; n < num_cmd; n++)
        {
            bool ret = strcmp_cmd(begining_cli, (char*)commands_list[n]->cmd);
            if(ret){
                break;
            }
        }
        
        // We coudn't find any primary commands
        if((n+1) == num_cmd){
            continue;
        }else{
            int k;
            begining_cli = find_next_space_separeted_string(begining_cli);
            for(k = 0; k < commands_list[n]->cli_list_len; k++){
                bool ret = strcmp_cmd(begining_cli, (char*)commands_list[n]->cli_list[k].cmd);
                if(ret){
                    begining_cli = find_next_space_separeted_string(begining_cli);
                    commands_list[n]->cli_list[k].cli_func(begining_cli, strlen(begining_cli));
                }
            }
        }

        os_thread_sleep_ms(10);
    }
}
#endif