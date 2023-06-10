
#include "unit_check.h"
#include "safe_circular_queue.h"
#include "global_includes.h"

//#define CIRCULAR_QUEUE_DEBUGGING

#ifdef CIRCULAR_QUEUE_DEBUGGING
#define circular_println(e) println(e)
#else
#define circular_println(e) void(e)
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

    void *data_ptr = (void*)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->head), 4);
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

    os_mut_entry_wait_indefinite(&queue->queue_mutx);

    void *data_ptr = (void*)align_up((intptr_t)queue->data_ptr + (queue->element_size * queue->tail), 4);

    memcpy(element, data_ptr, element_size);

    queue->tail++;
    queue->num_elements_in_queue--;

    if (queue->tail == queue->num_elements)
        queue->tail = 0;

    os_mut_exit(&queue->queue_mutx);

    if (os_mut_count(&queue->enqueue_mutex) == 1)
        os_mut_exit(&queue->enqueue_mutex);
    return OS_RET_OK;
}

int safe_circular_dequeue_notimeout(safe_circular_queue_t *queue, size_t element_size, void *element)
{
    int ret = safe_circular_dequeue(queue, element_size, element);
    if (ret == OS_RET_OK)
    {
        circular_println("List has item");
        return ret;
    }
    else if (ret == OS_RET_LIST_EMPTY)
    {
        circular_println("List is empty.. waiting");
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
    
    os_mut_deinit(&queue->dequeue_mutex);
    os_mut_deinit(&queue->enqueue_mutex);
    os_mut_deinit(&queue->queue_mutx);

    return OS_RET_OK;
}

safe_circular_queue_t queue;

typedef struct {
    int n_one;
    uint8_t n_two;
    short n_three;
    float n_four;
}test_struct_t;

int safe_circular_queue_unit_test(void)
{
    unit_test_mod_init();

    int ret = safe_circular_queue_init(&queue, 128, sizeof(test_struct_t));
    assert_testcase_equal("Circular Queue Init", ret, OS_RET_OK);
    
    test_struct_t src;
    test_struct_t testgate[128];

    for (int n = 0; n < 128; n++)
    {

        src.n_one = n;
        src.n_two = 255-n;
        src.n_three = UINT16_MAX - n;
        src.n_four = 1024/(float)n;
        testgate[n] = src;

        ret = safe_circular_enqueue_notimeout(&queue, sizeof(src), &src);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    }

    ret = safe_circular_enqueue(&queue, sizeof(src), &src);
    assert_testcase_equal("enqueue fail ret status", ret, OS_RET_LOW_MEM_ERROR);

    for (int n = 0; n < 128; n++)
    {
        ret = safe_circular_dequeue_notimeout(&queue, sizeof(src), &src);

        if(src.n_one == testgate[n].n_one){
            assert_testcase_equal("dequeue n_one_test", 0, 0);
            
        }
        else{
            assert_testcase_equal("dequeue n_one_test", 0, 1);
        }

        if(src.n_two == testgate[n].n_two){
            assert_testcase_equal("dequeue n_two_test", 0, 0);
        }
        else{
            assert_testcase_equal("dequeue n_two_test", 0, 1);
        }

        if(src.n_three == testgate[n].n_three){
            assert_testcase_equal("dequeue n_three_test", 0, 0);
        }
        else{
            assert_testcase_equal("dequeue n_three_test", 0, 1);
        }

        if(src.n_four == testgate[n].n_four){
            assert_testcase_equal("dequeue n_four_test", 0, 0);
        }
        else{
            assert_testcase_equal("dequeue n_four_test", 0, 1);
        }
    }    
    ret = safe_circular_deinit(&queue);
    assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);

    unit_testcase_end();
    return OS_RET_OK;
}