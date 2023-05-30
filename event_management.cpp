#include "event_management.h"
#include "threads_list.h"
#include "os_error.h"
#include "../os_mutx.h"

static os_mut_t event_queue_head_mut;
static event_type_queue_ll_t *event_queue_head = NULL;
static bool inited = false;

void event_management_init(void *params){
    int num_events = sizeof(event_type_t);

    event_queue_head = (event_type_queue_ll_t*)malloc(sizeof(event_type_queue_ll_t));

    // Generate linked list to iterate through when publishing/subscribing events
    for(int n = 0; n < num_events; n++){
       event_queue_head->event_id = (event_type_t)n;
       event_queue_head->next = (event_type_queue_ll_t*)malloc(sizeof(event_type_queue_ll_t));
    }

    // NULL terminater
    event_queue_head->next = NULL;

    // Generate and clear the mutex around the entire mutex list
    os_mut_init(&event_queue_head_mut);
    os_mut_exit(&event_queue_head_mut);
    inited = true;
}

int subscribe_event(local_event_queue_t *local_eventqueue, event_type_t event){
    if(inited == false){
        return OS_RET_NOT_INITIALIZED;
    }

    if(local_eventqueue == NULL){
        return OS_RET_INVALID_PARAM;
    }

    // Iterate through eventlist to find the correct event
    // We need to subscribe to
    event_type_queue_ll_t *node = event_queue_head;
    while(node != NULL && node->event_id != event){
        node = node->next;
    }

    // How you got here idk, there should be a list for every event
    if(node == NULL){
        return OS_RET_INT_ERR;
    }

    os_mut_entry_wait_indefinite(&event_queue_head_mut);
    // First item in list
    if(node->local_event_queue_head == NULL){
        node->local_event_queue_head = (local_event_queue_ll_t*)malloc(sizeof(local_event_queue_ll_t));
        node->local_event_queue_head->queue = local_eventqueue;
        node->local_event_queue_head->next = NULL;
    }
    
    // Otherwise we iterate through the list
    local_event_queue_ll_t *local_node = node->local_event_queue_head;
    while(local_node->next != NULL){
        // If it's already in the list we return out
        if(local_node->queue == local_eventqueue){
            os_mut_exit(&event_queue_head_mut);
            return OS_RET_ALREADY_INITED;
        }
        local_node = local_node->next;
    }

    local_node->next = (local_event_queue_ll_t*)malloc(sizeof(local_event_queue_ll_t));
    local_node = local_node->next;
    // Populate with our local eventqueue
    local_node->queue = local_eventqueue;
    // NULL terminate
    local_node->next = NULL;

    // Exit lock
    os_mut_exit(&event_queue_head_mut);
    return OS_RET_OK;
}

local_event_queue_t* new_local_eventqueue(void){
    local_event_queue_t *queue = (local_event_queue_t*)malloc(sizeof(local_event_queue_t));

    int ret = os_mut_init(&queue->local_queue_mutex);
    if(ret != OS_RET_OK){
        return NULL;
    }    
    
    os_mut_exit(&queue->local_queue_mutex);

    //queue->safe_circular_queue;
    return queue;
}

void event_management_thread(void *parameters){
    for(;;){

    }
}

int subscribe_eventlist(local_event_queue_t *local_eventqueue, int *event_list, int num_events, int event_length_max){
    for(int n = 0; n < num_events; n++){
        int ret = subscribe_event(local_eventqueue, (event_type_t)event_list[n]);
        if(ret != OS_RET_OK){
            return ret;
        }
    }
    return OS_RET_OK;
}

bool available_events(int eventspace){

    return false;
}

event_data_t consume_event(int eventspace){
    event_data_t data;

    return data;
}