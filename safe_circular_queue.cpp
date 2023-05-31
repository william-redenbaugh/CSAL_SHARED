#include "safe_circular_queue.h"
#include "unit_check.h"

#ifndef align_up
#define align_up(num, align) \
    (((num) + ((align) - 1)) & ~((align) - 1))
#endif

int safe_circular_queue_init(safe_circular_queue_t *queue, int num_elements, size_t element_size)
{
    int ret;
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    ret = os_mut_init(&queue->queue_mutx);

    if (ret != OS_RET_OK)
    {
        queue->status = OS_STATUS_FAILED_INIT;
        return ret;
    }

    ret = os_mut_init(&queue->dequeue_mutex);

    if (ret != OS_RET_OK)
    {
        queue->status = OS_STATUS_FAILED_INIT;
        return ret;
    }

    ret = os_mut_init(&queue->enqueue_mutex);

    if (ret != OS_RET_OK)
    {
        queue->status = OS_STATUS_FAILED_INIT;
        return ret;
    }

    if(num_elements == 0 || element_size == 0){
        return OS_RET_INVALID_PARAM;
    }

    // Align memory to closest 32 bit integer(assuming we are a 32bit system for now...  cross this bridge later heh)
    element_size = align_up(element_size, 4);
    int total_memory = element_size * num_elements;
    Log.info(" Circular Queue: Malloced size: %d", total_memory);
    queue->data_ptr = malloc(total_memory);

    // memset(queue->data_ptr, 0, element_size * num_elements);
    if (queue->data_ptr == NULL)
    {
        queue->status = OS_STATUS_FAILED_INIT;
        return OS_RET_LOW_MEM_ERROR;
    }

    queue->element_size = element_size;
    queue->num_elements = num_elements;
    queue->head = 0;
    queue->tail = 0;
    queue->num_elements_in_queue = 0;
    queue->status = OS_STATUS_INITIALIZED;

    return OS_RET_OK;
}

int safe_circular_enqueue(safe_circular_queue_t *queue, size_t element_size, void *element)
{
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    // Want alignment to power of 4! otherwise it's a failiure
    if(element_size < 4){
        return OS_RET_INVALID_PARAM;
    }

    if (element_size != queue->element_size)
    {
        return OS_RET_INVALID_PARAM;
    }

    if (queue->num_elements_in_queue >= queue->num_elements)
    {
        return OS_RET_LOW_MEM_ERROR;
    }

    // Wraparound queue function
    os_mut_entry_wait_indefinite(&queue->queue_mutx);

    void *data_ptr = (void*)align_up((int)queue->data_ptr + (queue->element_size * queue->head), 4);
    memcpy(data_ptr, element, queue->element_size);

    queue->head++;
    queue->num_elements_in_queue++;

    if (queue->head == queue->num_elements)
        queue->head = 0;

    os_mut_exit(&queue->queue_mutx);

    // Signal that there's an event to be consumed if anyone is blocking!
    if (os_mut_count(&queue->dequeue_mutex) == 1)
        os_mut_exit(&queue->dequeue_mutex);

    return OS_RET_OK;
}

int safe_circular_enqueue_timeout(safe_circular_queue_t *queue, size_t element_size, void *element, uint32_t timeout_ms)
{
    int ret = safe_circular_enqueue(queue, element_size, element);
    if (ret == OS_RET_OK)
    {
        return ret;
    }
    else if (ret == OS_RET_LOW_MEM_ERROR)
    {
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->enqueue_mutex);
        // Wait up to a specific timeout
        ret = os_mut_entry(&queue->enqueue_mutex, timeout_ms);
        if (ret != OS_RET_OK)
        {
            return ret;
        }
        return safe_circular_enqueue(queue, element_size, element);
    }
    else
    {
        return ret;
    }
}

int safe_circular_enqueue_notimeout(safe_circular_queue_t *queue, size_t element_size, void *element)
{
    int ret = safe_circular_enqueue(queue, element_size, element);
    if (ret == OS_RET_OK)
    {
        return ret;
    }
    else if (ret == OS_RET_LOW_MEM_ERROR)
    {
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->enqueue_mutex);
        // Wait up to a specific timeout
        ret = os_mut_entry_wait_indefinite(&queue->enqueue_mutex);
        if (ret != OS_RET_OK)
        {
            return ret;
        }
        return safe_circular_enqueue(queue, element_size, element);
    }
    else
    {
        return ret;
    }
}

int safe_circular_dequeue(safe_circular_queue_t *queue, size_t element_size, void *element)
{
    if (queue == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    if (element_size != queue->element_size)
    {
        return OS_RET_INVALID_PARAM;
    }

    if (queue->num_elements_in_queue == 0)
    {
        return OS_RET_LIST_EMPTY;
    }

    //os_mut_entry_wait_indefinite(&queue->queue_mutx);

    void *data_ptr = queue->data_ptr + (queue->tail * queue->element_size);

    memcpy(element, data_ptr, element_size);

    queue->tail++;
    queue->num_elements_in_queue--;

    if (queue->tail == queue->num_elements)
        queue->tail = 0;

    //os_mut_exit(&queue->queue_mutx);

    if (os_mut_count(&queue->enqueue_mutex) == 1)
        os_mut_exit(&queue->enqueue_mutex);
    return OS_RET_OK;
}

int safe_circular_dequeue_notimeout(safe_circular_queue_t *queue, size_t element_size, void *element)
{
    int ret = safe_circular_dequeue(queue, element_size, element);
    if (ret == OS_RET_OK)
    {
        return ret;
    }
    else if (ret == OS_RET_LIST_EMPTY)
    {
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->dequeue_mutex);
        // wait until we dequeue
        ret = os_mut_entry_wait_indefinite(&queue->dequeue_mutex);
        if (ret != OS_RET_OK)
        {
            return ret;
        }
        return safe_circular_dequeue(queue, element_size, element);
    }
    else
    {
        return ret;
    }
}

int safe_circular_dequeue_timeout(safe_circular_queue_t *queue, size_t element_size, void *element, uint32_t timeout_ms)
{
    int ret = safe_circular_dequeue(queue, element_size, element);
    if (ret == OS_RET_OK)
    {
        return ret;
    }

    else if (ret == OS_RET_LIST_EMPTY)
    {
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->dequeue_mutex);
        // wait until we dequeue
        ret = os_mut_entry(&queue->dequeue_mutex, timeout_ms);
        if (ret != OS_RET_OK)
        {
            return ret;
        }
        return safe_circular_dequeue(queue, element_size, element);
    }
    else
    {
        return ret;
    }
}

int safe_circular_deinit(safe_circular_queue_t *queue){
    if(queue == NULL){
        return OS_RET_NULL_PTR;
    }
    if(queue->status == OS_STATUS_UNINITIALIZED){
        return OS_RET_NOT_INITIALIZED;
    }

    if(queue->data_ptr == NULL){
        return OS_RET_INT_ERR;
    }

    free(queue->data_ptr);
    queue->head = 0;
    queue->tail = 0;
    queue->status = OS_STATUS_UNINITIALIZED;
    queue->num_elements = 0;
    queue->num_elements_in_queue = 0;
    
    queue->dequeue_mutex;
    queue->enqueue_mutex;
    queue->queue_mutx;

    return OS_RET_OK;
}

static safe_circular_queue_t queue;
int safe_circular_queue_unit_test(void)
{
    unit_test_mod_init();
    delay(2000);
    Log.info("Free Memory: %lu\n", System.freeMemory());

    int ret = safe_circular_queue_init(&queue, 256, sizeof(int));
    assert_testcase_equal("Circular Queue Init", ret, OS_RET_OK);

    int n = 15;
    int k;

    Log.info("Free Memory: %lu\n", System.freeMemory());
    for (n = 0; n < 32; n++)
    {
        ret = safe_circular_enqueue(&queue, sizeof(n), &n);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
        Log.info("%d", n);
    }

    for (int n = 0; n < 32; n++)
    {
        ret = safe_circular_dequeue(&queue, sizeof(n), &k);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
        Log.info("%d", k);
    }

    /*
    for(int n = 0; n < 4; n++){
        ret = safe_circular_dequeue(&queue, sizeof(n), &k);
        assert_testcase_equal("dequeue no timeout ret status", ret, OS_RET_OK);
        Log.info("%d %d", k, n);
        assert_testcase_equal("safe_circular_dequeue_notimeout value", k, n);
    }
    /*

    for(n = 0; n < 5; n++){
        ret = safe_circular_enqueue(&queue, sizeof(n), &n);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    }

    //ret = safe_circular_enqueue(&queue, sizeof(n), &n);
    //assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    */
    unit_testcase_end();
    return OS_RET_OK;
}