
#ifndef THREADS_INIT_H
#define THREADS_INIT_H

#include "stdint.h"
#include "platform_cshal.h"
/**
 * @brief Structure that manages task initialization.
 * @note Designed to help intiailize lists of tasks all at once.
 */
typedef struct task_init_descriptor_t
{
    void (*task_fun)(void *task_params);
    void (*task_init_fun)(void *task_params);
    const char *task_name;
    uint16_t stack_size;
    uint8_t *stack;
    uint16_t priority;
    void *param;
    os_thread_id_t handle;
} task_init_descriptor_t;

/**
 * @brief Initialize all the threads that we put into our list.
 *
 */
extern void threads_list_init(void);

#endif
