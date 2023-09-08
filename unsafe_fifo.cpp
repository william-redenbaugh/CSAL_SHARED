#include "unsafe_fifo.h"
#include "os_error.h"
#include "string.h"

/**
 * Initializes an unsafe FIFO queue.
 *
 * @param fifo A pointer to the uninitialized `unsafe_fifo_t` structure.
 * @param num_elements The maximum number of elements the queue can hold.
 * @param element_size The size of each element in bytes.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_queue_init(unsafe_fifo_t *fifo, int num_elements, size_t element_size)
{
    if (fifo == NULL || num_elements <= 0 || element_size <= 0)
    {
        return OS_RET_INVALID_PARAM; // Invalid input parameters.
    }

    // Calculate the size of the aligned data buffer.
    size_t aligned_size = num_elements * element_size;
    aligned_size = (aligned_size + 3) & ~3; // Align to 4 bytes (32 bits).

    fifo->data_ptr = malloc(aligned_size);
    if (fifo->data_ptr == NULL)
    {
        return OS_RET_LOW_MEM_ERROR; // Memory allocation failed.
    }

    fifo->num_elements = num_elements;
    fifo->element_size = element_size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->num_elements_in_queue = 0;

    return OS_RET_OK; // Initialization successful.
}

/**
 * Enqueues an element into the unsafe FIFO queue.
 *
 * @param queue A pointer to the initialized `unsafe_fifo_t` structure.
 * @param element_size The size of the element being enqueued in bytes.
 * @param element A pointer to the element to be enqueued.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_enqueue(unsafe_fifo_t *queue, size_t element_size, void *element)
{
    if (queue == NULL || element == NULL || element_size != queue->element_size)
    {
        return OS_RET_INVALID_PARAM; // Invalid input parameters.
    }

    if (queue->num_elements_in_queue == queue->num_elements)
    {
        return OS_RET_LOW_MEM_ERROR; // The queue is full.
    }

    // Calculate the aligned offset for the enqueue operation.
    size_t aligned_offset = (queue->tail * element_size);
    aligned_offset = (aligned_offset + 3) & ~3; // Align to 4 bytes (32 bits).

    // Copy the element into the queue's data buffer with alignment.
    char *data = (char *)queue->data_ptr;
    data += aligned_offset;
    memcpy(data, element, element_size);

    // Update queue properties.
    queue->tail = (queue->tail + 1) % queue->num_elements;
    queue->num_elements_in_queue++;

    return OS_RET_OK; // Enqueue successful.
}

/**
 * Dequeues an element from the unsafe FIFO queue.
 *
 * @param queue A pointer to the initialized `unsafe_fifo_t` structure.
 * @param element_size The size of the element being dequeued in bytes.
 * @param element A pointer to where the dequeued element will be stored.
 * @return 0 on success, or a negative value on failure.
 */
int unsafe_fifo_dequeue(unsafe_fifo_t *queue, size_t element_size, void *element)
{
    if (queue == NULL || element == NULL || element_size != queue->element_size)
    {
        return OS_RET_INVALID_PARAM; // Invalid input parameters.
    }

    if (queue->num_elements_in_queue == 0)
    {
        return OS_RET_LIST_EMPTY; // The queue is empty.
    }

    // Calculate the aligned offset for the dequeue operation.
    size_t aligned_offset = (queue->head * element_size);
    aligned_offset = (aligned_offset + 3) & ~3; // Align to 4 bytes (32 bits).

    // Copy the element from the queue's data buffer with alignment.
    char *data = (char *)queue->data_ptr;
    data += aligned_offset;
    memcpy(element, data, element_size);

    // Update queue properties.
    queue->head = (queue->head + 1) % queue->num_elements;
    queue->num_elements_in_queue--;

    return OS_RET_OK; // Dequeue successful.
}