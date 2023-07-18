#ifndef _OS_QUEUE_H
#define _OS_QUEUE_H

#include "global_includes.h"

typedef struct csal_queue_data_t
{
    void *data;
} csal_queue_data_t;

typedef struct csal_queue_t
{
    os_mut_t queue_lock;
    int head;
    int tail;
    int max_num_elemnts;
    int current_num_elements;
    void **data;
} csal_queue_t;

/**
 * @brief Initiualizes OS Blocking/Threadsafe Queue
 * @param csal_queue_t *pointer to queue
 * @param int maximum elements
 */
int os_queue_init(csal_queue_t *queue, int max_elements);

/**
 * @brief Pushes an element into the queue
 * @note Will return if we were able to push to the queue
 * @param csal_queue_t *pointer to queue
 * @param void *ptr to data
 * @param int timeout in milliseconds
 */
int os_queue_push_timeout(csal_queue_t *queue, void *data, int timeout_ms);

/**
 * @brief Pushes element, waiting indefinitely if there's too many items in the list already
 * @param csal_queue_t *pointer to queue
 * @param void* data pointer
 */
int os_queue_push_indefinite(csal_queue_t *queue, void *data);

/**
 * @brief Pops element from the list
 * @param csal_queue_t *pointer to queue
 * @param int timeout_ms
 */
void *os_queue_pop_timeout(csal_queue_t *queue, int timeout_ms, int *ret);

/**
 * @brief Waits indefinintely until there's an item on the list to pop off
 * @param csal_queue_t *pointer to queue structure
 */
void *os_queue_pop_indefinite(csal_queue_t *queue, int *ret);

#endif