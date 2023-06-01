#include "threads_init.h"
#include "threads_list.h"
#include "../os_misc.h"

int  NUM_THREADS = sizeof(INIT_THREAD_LIST)/sizeof(task_init_descriptor_t);

void threads_list_init(void)
{
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(INIT_THREAD_LIST[n].task_init_fun!= NULL)
            INIT_THREAD_LIST[n].task_init_fun(NULL);
        println("Initialized another module");
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(INIT_THREAD_LIST[n].task_fun != NULL){
            // New block of memory from heap for stack pointer
            uint8_t *stack_ptr;
            if(INIT_THREAD_LIST[n].stack == NULL){
                //stack_ptr = new uint8_t[INIT_THREAD_LIST[n].stack_size];
            }
            else{
                stack_ptr = INIT_THREAD_LIST[n].stack;
            }
            os_add_thread((thread_func_t)INIT_THREAD_LIST[n].task_fun, INIT_THREAD_LIST[n].param, INIT_THREAD_LIST[n].stack_size, stack_ptr);
            println("Initialized another thread");
        }
    }
}