#include "threads_init.h"
#include "threads_list.h"

// #define THREADS_INIT_DEBUGGING
#ifdef THREADS_INIT_DEBUGGING
#define thread_init_debugging(e) print(e)
#else
#define thread_init_debugging(e) void(e)
#endif

int NUM_THREADS = sizeof(INIT_THREAD_LIST) / sizeof(task_init_descriptor_t);

void threads_list_init(void)
{

    for (int n = 0; n < NUM_THREADS; n++)
    {
        thread_init_debugging((char *)INIT_THREAD_LIST[n].task_name);
        if (INIT_THREAD_LIST[n].task_init_fun != NULL)
            INIT_THREAD_LIST[n].task_init_fun(NULL);
        thread_init_debugging("Initialized another module");
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if (INIT_THREAD_LIST[n].task_fun != NULL)
        {
            // New block of memory from heap for stack pointer
            uint8_t *stack_ptr = NULL;
            if (INIT_THREAD_LIST[n].stack == NULL)
            {
                // stack_ptr = new uint8_t[INIT_THREAD_LIST[n].stack_size];
            }
            else
            {
                stack_ptr = INIT_THREAD_LIST[n].stack;
            }

            os_add_thread((thread_func_t)INIT_THREAD_LIST[n].task_fun, INIT_THREAD_LIST[n].param, INIT_THREAD_LIST[n].stack_size, stack_ptr);
            thread_init_debugging("Initialized another thread");
        }
    }
}