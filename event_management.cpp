#include "event_management.h"
#include "threads_list.h"
#include "os_error.h"
#include "../os_misc.h"
#include "../os_mutx.h"

#define PUBLISH_EVENT_QUEUE_MAX_SIZE 16
#define PER_QUEUE_MAX_SIZE 16

// #define EVENT_MANAGEMENT_DEBUGGING
#ifdef EVENT_MANAGEMENT_DEBUGGING
#define event_management_println(e) println(e)
#else
#define event_management_println(e) (void)e
#endif

static os_mut_t event_queue_head_mut;
static event_type_queue_ll_t *event_queue_head = NULL;
static bool inited = false;
safe_circular_queue_t publish_event_queue;

void event_management_init(void *params){
    int num_events = EVENT_TYPE_EVENT_END;

    event_queue_head = (event_type_queue_ll_t*)malloc(sizeof(event_type_queue_ll_t));
    event_type_queue_ll_t *node = event_queue_head;

    // Generate linked list to iterate through when publishing/subscribing events
    for(int n = 0; n < num_events; n++){
       node->event_id = (event_type_t)n;
       node->local_event_queue_head = NULL;
       node->next = (event_type_queue_ll_t*)malloc(sizeof(event_type_queue_ll_t));
       node = node->next;
       event_management_println("Event generated initialized");
    }

    // NULL terminater
    node->next = NULL;

    // Generate and clear the mutex around the entire mutex list
    os_mut_init(&event_queue_head_mut);
    os_mut_exit(&event_queue_head_mut);

    if(safe_circular_queue_init(&publish_event_queue, PUBLISH_EVENT_QUEUE_MAX_SIZE, sizeof(event_data_t)) != OS_RET_OK){
        println((char*)"Circular Queue Failed to initialize");
    }
    
    inited = true;
}

int subscribe_event(local_event_queue_t *local_eventqueue, event_type_t event){
    if(local_eventqueue->eventqueue_status != OS_STATUS_INITIALIZED){
        return OS_RET_NOT_INITIALIZED;
    }

    if(inited == false){
        return OS_RET_NOT_INITIALIZED;
    }

    if(local_eventqueue == NULL){
        return OS_RET_INVALID_PARAM;
    }

    event_management_println("Subscribing to event");
    // Iterate through eventlist to find the correct event
    // We need to subscribe to
    event_type_queue_ll_t *node = event_queue_head;
    while(node != NULL && node->event_id != event){
        node = node->next;
    }

    // How you got here idk, there should be a list for every event
    if(node == NULL){
        event_management_println("Couldn't find correct node for event");
        return OS_RET_INT_ERR;
    }

    event_management_println("Found correct node for event");
    
    os_mut_entry_wait_indefinite(&event_queue_head_mut);
    // First item in list created. 
    if(node->local_event_queue_head == NULL){
        node->local_event_queue_head = (local_event_queue_ll_t*)malloc(sizeof(local_event_queue_ll_t));
        node->local_event_queue_head->queue = local_eventqueue;
        node->local_event_queue_head->next = NULL;
        event_management_println("First node in list");
    }
    else{
        // Otherwise we iterate through the list
        local_event_queue_ll_t *local_node = node->local_event_queue_head;
        while(local_node->next != NULL){
            event_management_println("Not first node .. iterating...");
            // If it's already in the list we return out
            if(local_node->queue == local_eventqueue){
                os_mut_exit(&event_queue_head_mut);
                return OS_RET_ALREADY_INITED;
            }
            local_node = local_node->next;
        }

        event_management_println("First node in list");
        local_node->next = (local_event_queue_ll_t*)malloc(sizeof(local_event_queue_ll_t));
        local_node = local_node->next;
        // Populate with our local eventqueue
        local_node->queue = local_eventqueue;
        // NULL terminate
        local_node->next = NULL;
    }
    // Exit lock
    os_mut_exit(&event_queue_head_mut);
    return OS_RET_OK;
}

local_event_queue_t* new_local_eventqueue(int num_elements_queue){
    local_event_queue_t *queue = (local_event_queue_t*)malloc(sizeof(local_event_queue_t));
    queue->eventqueue_status = OS_STATUS_INITIALIZED;

    int ret = os_mut_init(&queue->local_queue_mutex);
    if(ret != OS_RET_OK){
        event_management_println("Wasn't able to initialize another eventqueue mutex");
        queue->eventqueue_status = OS_STATUS_MODULE_FAILED;
        return queue;
    }    
    
    os_mut_exit(&queue->local_queue_mutex);
    if(num_elements_queue == 0){
        num_elements_queue = PER_QUEUE_MAX_SIZE;
    }

    ret = safe_circular_queue_init(&queue->event_queue, num_elements_queue, sizeof(event_data_t));
    if(ret != OS_RET_OK){
        event_management_println("Wasn't able to initialize another eventqueue circular queue");
        queue->eventqueue_status = OS_STATUS_MODULE_FAILED;
        return queue;
    }
    return queue;
}

int publish_event(int event, void *ptr){
    if(event > EVENT_TYPE_EVENT_END){
        return OS_RET_INVALID_PARAM;
    }

    event_data_t data = {
        .event_id = (event_type_t)event,
        .data_ptr = ptr
    };

    // Enqueue to our global eventqueue
    int ret = safe_circular_enqueue_notimeout(&publish_event_queue, sizeof(event_data_t), &data);
    if(ret != OS_RET_OK){
        return ret;
    }

    return OS_RET_OK;
}

void event_management_thread(void *parameters){
    for(;;){

        begining: 

        event_data_t data;
        // Sit and wait until we get data
        safe_circular_dequeue_notimeout(&publish_event_queue, sizeof(data), &data);
        event_management_println("Got an event from queue");

        os_mut_entry_wait_indefinite(&event_queue_head_mut);
        // Find the correct event that we just got published
        event_type_queue_ll_t *node = event_queue_head;
        
        while(node != NULL){
            if(node->event_id == data.event_id){
                event_management_println("Found correct event");
                break;
            }

            node = node->next;
        }
    
        // If we somehow couldn't find anyone subcribed to those events
        if(node == NULL){
            os_mut_exit(&event_queue_head_mut);
            goto begining;
        }

        // Iterated through all subscribed lists of that head
        // And add the event to their own localized eventqueue
        local_event_queue_ll_t *head = node->local_event_queue_head;

        os_mut_exit(&event_queue_head_mut);
        while(head != NULL){            
            os_mut_entry_wait_indefinite(&head->queue->local_queue_mutex); 
            safe_circular_enqueue_notimeout(&head->queue->event_queue, sizeof(data), &data);
            event_management_println("Submitting event to local queue");
            os_mut_exit(&head->queue->local_queue_mutex);
            head = head->next;
        }
    }
}

int subscribe_eventlist(local_event_queue_t *local_eventqueue, int *event_list, int num_events, int event_length_max){
    if(local_eventqueue->eventqueue_status != OS_STATUS_INITIALIZED){
        return OS_RET_NOT_INITIALIZED;
    }
    for(int n = 0; n < num_events; n++){
        event_management_println("subcribed event in list");
        int ret = subscribe_event(local_eventqueue, (event_type_t)event_list[n]);
        if(ret != OS_RET_OK){
            return ret;
        }
    }
    return OS_RET_OK;
}

bool available_events(local_event_queue_t *local_eventqueue){
    if(local_eventqueue == NULL){
        return false; 
    }
    
    if(local_eventqueue->eventqueue_status != OS_STATUS_INITIALIZED){
        return false;
    }
    
    return (local_eventqueue->event_queue.num_elements_in_queue > 0);
}

event_data_t consume_event(local_event_queue_t *local_eventqueue){
    event_data_t data;
    data.data_ptr = NULL;
    data.event_id = EVENT_NONE;

    if(local_eventqueue == NULL){
        return data; 
    }

    if(local_eventqueue->eventqueue_status != OS_STATUS_INITIALIZED){
        return data;
    }

    safe_circular_dequeue_notimeout(&local_eventqueue->event_queue, sizeof(data), &data); 
    return data;
}