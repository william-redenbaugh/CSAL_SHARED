#ifndef _UNSAFE_FIFO_H
#define _UNSAFE_FIFO_H

#include "stdint.h"
#include "stdlib.h"

/**
 * @struct unsafe_fifo_t
 * @brief A data structure representing an unsafe First-In-First-Out (FIFO) queue.
 */
typedef struct unsafe_fifo_t
{
    void *data_ptr;            /**< Pointer to the data buffer. */
    int num_elements;          /**< The maximum number of elements the queue can hold. */
    size_t element_size;       /**< The size of each element in bytes. */
    int head;                  /**< Index of the head (front) of the queue. */
    int tail;                  /**< Index of the tail (end) of the queue. */
    int num_elements_in_queue; /**< The current number of elements in the queue. */
} unsafe_fifo_t;

/**
 * @brief Initializes an unsafe FIFO queue.
 *
 * This function initializes the unsafe FIFO queue by allocating memory for the data buffer
 * and setting the initial values for the queue's properties.
 *
 * @param fifo A pointer to the uninitialized `unsafe_fifo_t` structure.
 * @param num_elements The maximum number of elements the queue can hold.
 * @param element_size The size of each element in bytes.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_queue_init(unsafe_fifo_t *fifo, int num_elements, size_t element_size);

/**
 * @brief Enqueues an element into the unsafe FIFO queue.
 *
 * This function adds an element to the end of the queue.
 *
 * @param queue A pointer to the initialized `unsafe_fifo_t` structure.
 * @param element_size The size of the element being enqueued in bytes.
 * @param element A pointer to the element to be enqueued.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_enqueue(unsafe_fifo_t *queue, size_t element_size, void *element);

/**
 * @brief Dequeues an element from the unsafe FIFO queue.
 *
 * This function removes an element from the front of the queue.
 *
 * @param queue A pointer to the initialized `unsafe_fifo_t` structure.
 * @param element_size The size of the element being dequeued in bytes.
 * @param element A pointer to where the dequeued element will be stored.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_dequeue(unsafe_fifo_t *queue, size_t element_size, void *element);

/**
 * Deinitializes an unsafe FIFO queue.
 *
 * @param fifo A pointer to the initialized `unsafe_fifo_t` structure.
 */
void unsafe_fifo_deinit(unsafe_fifo_t *fifo);
#endif