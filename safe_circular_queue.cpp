#include "safe_circular_queue.h"
#include "unit_check.h"

int safe_circular_queue_init(safe_circular_queue_t *queue, int num_elements, size_t element_size){
    int ret; 
    if(queue == NULL){
        return OS_RET_NULL_PTR;
    }   

    ret = os_mut_init(&queue->queue_mutx);

    if(ret != OS_RET_OK){
        queue->status = OS_STATUS_FAILED_INIT;
        return ret; 
    }

    ret = os_mut_init(&queue->dequeue_mutex);
    
    if(ret != OS_RET_OK){
        queue->status = OS_STATUS_FAILED_INIT;
        return ret; 
    }

    ret = os_mut_init(&queue->enqueue_mutex);
    
    if(ret != OS_RET_OK){
        queue->status = OS_STATUS_FAILED_INIT;
        return ret; 
    }
    
    queue->data_ptr = malloc(element_size * num_elements);

    if(queue->data_ptr == NULL){
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

int safe_circular_enqueue(safe_circular_queue_t * queue, size_t element_size, void *element){
    if(queue == NULL){
        return OS_RET_NULL_PTR;
    }

    if(element_size != queue->element_size){
        return OS_RET_INVALID_PARAM;
    }

    if(queue->num_elements_in_queue >= queue->num_elements){
        return OS_RET_LOW_MEM_ERROR;
    }

    // Wraparound queue function
    os_mut_entry_wait_indefinite(&queue->queue_mutx);
    void *data_ptr = queue->data_ptr + (queue->head * queue->num_elements);
    memcpy(data_ptr, element, element_size);

    queue->head++;
    queue->num_elements_in_queue++;
    os_mut_exit(&queue->queue_mutx);

    // Signal that there's an event to be consumed
    os_mut_exit(&queue->dequeue_mutex);
    return OS_RET_OK;
}


int safe_circular_enqueue_timeout(safe_circular_queue_t * queue, size_t element_size, void *element, uint32_t timeout_ms){
    int ret = safe_circular_enqueue(queue, element_size, element);
    if(ret == OS_RET_OK){
        return ret;
    }
    else if(ret == OS_RET_LOW_MEM_ERROR){
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->enqueue_mutex);
        // Wait up to a specific timeout
        ret = os_mut_entry(&queue->enqueue_mutex, timeout_ms);
        if(ret != OS_RET_OK){
            return ret;
        }
        return safe_circular_enqueue(queue, element_size, element);
    }
    else{
        return ret;
    }
}

int safe_circular_enqueue_notimeout(safe_circular_queue_t * queue, size_t element_size, void *element){
    int ret = safe_circular_enqueue(queue, element_size, element);
    if(ret == OS_RET_OK){
        return ret;
    }
    else if(ret == OS_RET_LOW_MEM_ERROR){
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->enqueue_mutex);
        // Wait up to a specific timeout
        ret = os_mut_entry_wait_indefinite(&queue->enqueue_mutex);
        if(ret != OS_RET_OK){
            return ret;
        }
        return safe_circular_enqueue(queue, element_size, element);
    }
    else{
        return ret;
    }
}

int safe_circular_dequeue(safe_circular_queue_t * queue, size_t element_size, void *element){
    if(queue == NULL){
        return OS_RET_NULL_PTR;
    }

    if(element_size != queue->element_size){
        return OS_RET_INVALID_PARAM;
    }

    if(queue->num_elements_in_queue == 0){
        return OS_RET_LIST_EMPTY;
    }

    os_mut_entry_wait_indefinite(&queue->queue_mutx);
    void *data_ptr = queue->data_ptr + (queue->tail * queue->num_elements);
    memcpy(element, data_ptr, element_size);

    queue->tail++;
    queue->num_elements_in_queue--;
    os_mut_exit(&queue->queue_mutx);

    // Signal that there's space for another item to be added to the queue
    os_mut_exit(&queue->enqueue_mutex);

    return OS_RET_OK;
}

int safe_circular_dequeue_notimeout(safe_circular_queue_t * queue, size_t element_size, void *element){
    int ret = safe_circular_dequeue(queue, element_size, element);
    if(ret == OS_RET_OK){
        return ret;
    }
    else if(ret == OS_RET_LOW_MEM_ERROR){
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->dequeue_mutex);
        // wait until we dequeue
        ret = os_mut_entry_wait_indefinite(&queue->dequeue_mutex);
        if(ret != OS_RET_OK){
            return ret;
        }
        return safe_circular_dequeue(queue, element_size, element);
    }
    else{
        return ret;
    }
}

int safe_circular_dequeue_timeout(safe_circular_queue_t * queue, size_t element_size, void *element, uint32_t timeout_ms){
    int ret = safe_circular_dequeue(queue, element_size, element);
    if(ret == OS_RET_OK){
        return ret;
    }
    else if(ret == OS_RET_LOW_MEM_ERROR){
        // Try to acquire the lock before we wait
        // That way it blocks until someone unlocks
        os_mut_try_entry(&queue->dequeue_mutex);
        // wait until we dequeue
        ret = os_mut_entry(&queue->dequeue_mutex, timeout_ms);
        if(ret != OS_RET_OK){
            return ret;
        }
        return safe_circular_dequeue(queue, element_size, element);
    }
    else{
        return ret;
    }
}

static safe_circular_queue_t queue;
int safe_circular_queue_unit_test(void){
    unit_test_mod_init();

    int ret = safe_circular_queue_init(&queue, 5, sizeof(int));
    int n = 15; 
    int k;
    
    ret = safe_circular_enqueue_notimeout(&queue, sizeof(n), &n);
    assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);

    ret = safe_circular_dequeue_notimeout(&queue, sizeof(n), &k);
    assert_testcase_equal("dequeue no timeout ret status", ret, OS_RET_OK);
    assert_testcase_equal("safe_circular_dequeue_notimeout value", k, n); 

    n = 69;

    ret = safe_circular_enqueue(&queue, sizeof(n), &n);
    assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    
    ret = safe_circular_dequeue(&queue, sizeof(n), &k);
    assert_testcase_equal("dequeue no timeout ret status", ret, OS_RET_OK);
    assert_testcase_equal("safe_circular_dequeue_notimeout value", k, n); 
    

    for(n = 0; n < 5; n++){
        ret = safe_circular_enqueue(&queue, sizeof(n), &n);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    }

    for(int n = 0; n < 5; n++){
        ret = safe_circular_dequeue(&queue, sizeof(n), &k);
        assert_testcase_equal("dequeue no timeout ret status", ret, OS_RET_OK);
        assert_testcase_equal("safe_circular_dequeue_notimeout value", k, n); 
    }

    for(n = 0; n < 5; n++){
        ret = safe_circular_enqueue(&queue, sizeof(n), &n);
        assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    }

    //ret = safe_circular_enqueue(&queue, sizeof(n), &n);
    //assert_testcase_equal("enqueue no timeout ret status", ret, OS_RET_OK);
    
    unit_testcase_end();
    return OS_RET_OK;
}