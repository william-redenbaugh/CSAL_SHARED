#include "local_eventqueue.h"
#include "os_error.h"

int local_eventqueue_init(local_eventqueue_t *eventqueue, int max_events){
    if (eventqueue == NULL){
        return OS_RET_NULL_PTR;
    }
    return safe_circular_queue_init(&eventqueue->internal_queue, max_events, sizeof(event_data_t));
}

int local_eventqueue_enqueue(local_eventqueue_t *eventqueue, event_data_t data){
    if (eventqueue == NULL){
        return OS_RET_NULL_PTR;
    }

    return safe_circular_enqueue_notimeout(&eventqueue->internal_queue, sizeof(event_data_t), &data);
}

int local_eventqueue_numevents(local_eventqueue_t *eventqueue, int *num_events){
    if (eventqueue == NULL){
        return OS_RET_NULL_PTR;
    }

    *num_events = eventqueue->internal_queue.num_elements_in_queue;
    return OS_RET_OK;
}

int local_eventqueue_peektop(local_eventqueue_t *eventqueue, event_data_t *data){
    if (eventqueue == NULL){
        return OS_RET_NULL_PTR;
    }

    return safe_circuclar_peektop(&eventqueue->internal_queue, sizeof(event_data_t), data);
}

int local_eventqueue_dequeue(local_eventqueue_t *eventqueue, event_data_t *data){
    if (eventqueue == NULL){
        return OS_RET_NULL_PTR;
    }
    
    return safe_circular_dequeue_notimeout(&eventqueue->internal_queue, sizeof(event_data_t), data);
}