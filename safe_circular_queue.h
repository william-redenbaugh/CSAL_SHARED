#ifndef _SAFE_CIRCULAR_QUEUE_H
#define _SAFE_CIRCULAR_QUEUE_H

#include "../os_mutx.h"
#include "os_error.h"
#include "../os_status.h"

typedef struct safe_circular_queue_t{
    os_mut_t queue_mutx;
    void *data_ptr;
    int num_elements;
    size_t element_size;
    os_status_t status;

    int head; 
    int tail;
    int num_elements_in_queue;
}safe_circular_queue_t;

/**
 * @brief Threadsafe Circular Queue Initialization
 * @param safe_circular_queue_t *pointer to queue descripter structure
 * @param int num_elements number of elements
 * @param size_t size of each element
*/
int safe_circular_queue_init(safe_circular_queue_t *queue, int num_elements, size_t element_size);

/**
 * @brief Theadsafe circular queue enque function
 * @param safe_circular_queue_t *pointer to queue descripter structure
 * @param size_t element_size size of element being parsed in
 * @param void *element pointer to element
 * @note the element_size is passed in as a check to make sure it's the same size as the item inside the circular queue
*/
int safe_circular_enqueue(safe_circular_queue_t * queue, size_t element_size, void *element);

/**
 * @brief Theadsafe circular queue deque function
 * @param safe_circular_queue_t *pointer to queue descripter structure
 * @param size_t element_size size of memory space of element being copied to
 * @param void *element pointer to memory space of element being copied into
 * @note the element_size is passed in as a check to make sure it's the same size as the item inside the circular queue
*/
int safe_circular_dequeue(safe_circular_queue_t * queue, size_t element_size, void *element);

#endif