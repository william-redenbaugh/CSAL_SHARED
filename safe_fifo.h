/**
 * @file safe_fifo.h
 * @brief Thread-safe FIFO (First-In-First-Out) queue implementation with synchronization mechanisms.
 */

#ifndef _SAFE_FIFO_H
#define _SAFE_FIFO_H

#include "os_setbits.h"
#include "stdint.h"
#include "os_mutx.h"

/**
 * @struct safe_fifo_t
 * @brief Thread-safe FIFO queue structure.
 */
typedef struct safe_fifo_t
{
    os_mut_t fifo_mutx; ///< Mutex for protecting the FIFO queue

    void *data_ptr;      ///< Pointer to the data buffer
    int num_elements;    ///< Maximum number of elements in the FIFO
    size_t element_size; ///< Size of each element in bytes

    uint32_t head;                  ///< Index of the head element
    uint32_t tail;                  ///< Index of the tail element
    uint32_t num_elements_in_queue; ///< Number of elements currently in the queue

    os_mut_t requested_data_mutx; ///< Mutex for protecting requested_data
    uint32_t requested_data;      ///< Number of requested data elements
} safe_fifo_t;

/**
 * @brief Initialize a safe FIFO queue.
 * @param queue Pointer to the safe_fifo_t instance to be initialized.
 * @param num_elements Maximum number of elements the queue can hold.
 * @param element_size Size of each element in bytes.
 * @return 0 if initialization is successful, otherwise a negative error code.
 */
int safe_fifo_init(safe_fifo_t *queue, int num_elements, size_t element_size);

/**
 * @brief Enqueue elements into the safe FIFO queue.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to enqueue.
 * @param element_list Pointer to an array of elements to enqueue.
 * @return Number of successfully enqueued elements, or a negative error code.
 */
int safe_fifo_enqueue(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

/**
 * @brief Enqueue elements into the safe FIFO queue with a timeout.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to enqueue.
 * @param element_list Pointer to an array of elements to enqueue.
 * @return Number of successfully enqueued elements, or a negative error code.
 */
int safe_fifo_enqueue_timeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

/**
 * @brief Enqueue elements into the safe FIFO queue without timeout.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to enqueue.
 * @param element_list Pointer to an array of elements to enqueue.
 * @return Number of successfully enqueued elements, or a negative error code.
 */
int safe_fifo_enqueue_notimeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

/**
 * @brief Dequeue elements from the safe FIFO queue.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to dequeue.
 * @param element_list Pointer to an array where dequeued elements will be stored.
 * @return Number of successfully dequeued elements, or a negative error code.
 */
int safe_fifo_dequeue(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

/**
 * @brief Dequeue elements from the safe FIFO queue with a timeout.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to dequeue.
 * @param element_list Pointer to an array where dequeued elements will be stored.
 * @return Number of successfully dequeued elements, or a negative error code.
 */
int safe_fifo_dequeue_timeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

/**
 * @brief Dequeue elements from the safe FIFO queue without timeout.
 * @param queue Pointer to the safe_fifo_t instance.
 * @param num_elements Number of elements to dequeue.
 * @param element_list Pointer to an array where dequeued elements will be stored.
 * @return Number of successfully dequeued elements, or a negative error code.
 */
int safe_fifo_dequeue_notimeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list);

#endif /* SAFE_FIFO_H */