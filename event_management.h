#ifndef _EVENT_MANAGEMENT_H
#define _EVENT_MANAGEMENT_H

#include "os_error.h"
#include "safe_circular_queue.h"
#include "os_status.h"
#include "event_type_list.h"

typedef struct{
    os_mut_t local_queue_mutex;
    safe_circular_queue_t event_queue;
    os_status_t eventqueue_status;
}local_event_queue_t;

typedef struct local_event_queue_ll_t{
    local_event_queue_t *queue; 
    struct local_event_queue_ll_t *next;
}local_event_queue_ll_t;

typedef struct event_type_queue_ll_t{
    // Event ID
    event_type_t event_id; 
    // Pointer to queue that we need to enqueue
    struct event_type_queue_ll_t *next;
    // Head of linked list of local event queue pointers
    local_event_queue_ll_t *local_event_queue_head;
}event_type_queue_ll_t;


#define EVENT_PEEK_TIMEOUT 0

/**
 * @brief Publish an event to the local eventspace
 *
 * @param event Enumerated type of event
 * @param ptr Random state pointer to be passed between threads
 * @return int
 */
int publish_event(int event, void *ptr);

/**
 * @brief Thread that will handle all of our event management stuff.
 *
 * @param parameters
 * @note To be handled by our threads_init
 */
void event_management_thread(void *parameters);

/**
 * @brief Initialization for event management
*/
void event_management_init(void *params);

/**
 * @brief Subscribe to a list oe
 *
 * @param event_list List event
 * @param num_events How many events are you subscribing to
 * @param event_length_max How many events do you want your queue to fit
 * @return int mk_err status if an error. Otherwise provides the index of the event_subscribe list. Keep that so that you can consume events that are published. Check out error.h to see more information
 * @note 1 Thread can only subscribe to a single list, implementation limitation
 */
int subscribe_eventlist(local_event_queue_t *local_eventqueue, int *event_list, int num_events, int event_length_max);

/**
 * @brief Subscribes to an event that can be published
 * @param local_event_queue_t *local_eventqueue
 * @param event_type_t event that we are subscribed to
*/
int subscribe_event(local_event_queue_t *local_eventqueue, event_type_t event);

/**
 * @brief An eventqueue to subscribe to events from
 * @note It's expected that any worker thread or module will have it's own local eventqueue
 * @note No two threads should share an eventqueue
 * @note A single thread can have multiple queues, but then you're consuming the same
 * event twice wasting resources and execution time
*/
local_event_queue_t* new_local_eventqueue(int num_elements_queue);

/**
 * @brief Checks to see if there are any events in the currently selected local eventspace
 *
 * @param eventspace
 * @return true there are events
 * @return false there are no events, or there was an internal error
 */
bool available_events(local_event_queue_t *local_eventqueue);

/**
 * @brief Sit and wait for events to come into an eventspace
 *
 * @param eventspace local eventspace index
 * @return event_data_t struct with all our event_data
 * @note Will return the EVENT_TYPE_NONE if there was no actual event returned
 */
event_data_t consume_event(local_event_queue_t *local_eventqueue);
#endif