#include "safe_fifo.h"
#include "global_includes.h"

int safe_fifo_init(safe_fifo_t *queue, int num_elements, size_t element_size)
{
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    queue->element_size = align_up(element_size, 4);

    if (queue->data_ptr != NULL)
    {
        free(queue->data_ptr);
    }

    queue->data_ptr = malloc(num_elements * element_size);
    queue->num_elements = num_elements;
    queue->head = 0;
    queue->tail = 0;
    queue->num_elements_in_queue = 0;

    int ret = os_mut_init(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_mut_init(&queue->requested_data_mutex);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    return os_setbits_init(&queue->data_ready_bits);
}

int safe_fifo_enqueue(safe_fifo_t *queue, uint32_t num_elements, void *element_list)
{
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    if (queue->num_elements_in_queue + num_elements >= queue->num_elements)
    {
        return OS_RET_LOW_MEM_ERROR;
    }

    if (queue->num_elements_in_queue >= queue->num_elements)
    {
        return OS_RET_LOW_MEM_ERROR;
    }

    // Wraparound queue function
    int ret = os_mut_entry_wait_indefinite(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    for (int n = 0; n < num_elements; n++)
    {
        void *data_ptr = (void *)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->head), 4);
        // Copy each element
        memcpy(data_ptr, element_list, queue->element_size);

        // Wraparound tail
        queue->head++;
        if (queue->head == queue->num_elements)
        {
            queue->head = 0;
        }
    }

    queue->num_elements_in_queue += num_elements;
    ret = os_mut_exit(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_mut_entry_wait_indefinite(&queue->requested_data_mutex);
    int req_data = queue->requested_data;
    queue->requested_data = 0;
    ret = os_mut_exit(&queue->requested_data_mutex);
    
    // If the requester has a specific lower bounds of data wanted
    if (req_data <= queue->num_elements_in_queue)
    {
        ret = os_setbits_signal(&queue->data_ready_bits, 1);
        if (ret != OS_RET_OK)
        {
            return ret;
        }
    }

    return os_setbits_signal(&queue->data_ready_bits, 1);
}

int safe_fifo_dequeue(safe_fifo_t *queue, uint32_t num_elements, void *element_list)
{

    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    if (queue->num_elements_in_queue == 0)
    {
        return OS_RET_LIST_EMPTY;
    }

    int ret = os_mut_entry_wait_indefinite(&queue->requested_data_mutex);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    if (num_elements > queue->requested_data)
    {
        num_elements = queue->requested_data;
    }
    queue->requested_data = 0;
    ret = os_waitbits_indefinite(&queue->data_ready_bits, 1);
    if (ret != OS_RET_OK)
    {
        int n = os_mut_exit(&queue->requested_data_mutex);
        if (n != OS_RET_OK)
        {
            return n;
        }
        return ret;
    }
    ret = os_mut_exit(&queue->requested_data_mutex);

    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_clearbits(&queue->data_ready_bits, 1);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_mut_entry_wait_indefinite(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    for (int n = 0; n < num_elements; n++)
    {
        void *data_ptr = (void *)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->tail), 4);

        memcpy(element_list, data_ptr, queue->element_size);

        queue->tail++;
        queue->num_elements_in_queue--;

        if (queue->tail == queue->num_elements)
            queue->tail = 0;
    }

    return os_mut_exit(&queue->fifo_mutx);
}

int safe_fifo_dequeue_notimeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list)
{

    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    int ret = os_mut_entry_wait_indefinite(&queue->requested_data_mutex);
    queue->requested_data = num_elements;
    os_mut_exit(&queue->requested_data_mutex);
    
    ret = os_waitbits_indefinite(&queue->data_ready_bits, 1);
    if (ret != OS_RET_OK)
    {
        int n = os_mut_exit(&queue->requested_data_mutex);
        if (n != OS_RET_OK)
        {
            return n;
        }
        return ret;
    }

    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_clearbits(&queue->data_ready_bits, 1);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_mut_entry_wait_indefinite(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    for (int n = 0; n < num_elements; n++)
    {
        void *data_ptr = (void *)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->tail), 4);

        memcpy(element_list, data_ptr, queue->element_size);

        queue->tail++;
        queue->num_elements_in_queue--;

        if (queue->tail == queue->num_elements)
            queue->tail = 0;
    }

    return os_mut_exit(&queue->fifo_mutx);
}

int safe_fifo_dequeue_timeout(safe_fifo_t *queue, uint32_t num_elements, void *element_list, uint32_t timeout_ms)
{
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    if (queue->num_elements_in_queue == 0)
    {
        return OS_RET_LIST_EMPTY;
    }

    int ret = os_mut_entry_wait_indefinite(&queue->requested_data_mutex);
    queue->requested_data = num_elements;
    ret = os_waitbits(&queue->data_ready_bits, 1, timeout_ms);
    if (ret != OS_RET_OK)
    {
        int n = os_mut_exit(&queue->requested_data_mutex);
        if (n != OS_RET_OK)
        {
            return n;
        }
        return ret;
    }
    ret = os_mut_exit(&queue->requested_data_mutex);

    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_clearbits(&queue->data_ready_bits, 1);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    ret = os_mut_entry_wait_indefinite(&queue->fifo_mutx);
    if (ret != OS_RET_OK)
    {
        return ret;
    }

    for (int n = 0; n < num_elements; n++)
    {
        void *data_ptr = (void *)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->tail), 4);

        memcpy(element_list, data_ptr, queue->element_size);

        queue->tail++;
        queue->num_elements_in_queue--;

        if (queue->tail == queue->num_elements)
            queue->tail = 0;
    }

    return os_mut_exit(&queue->fifo_mutx);
}