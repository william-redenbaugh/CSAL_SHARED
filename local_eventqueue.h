#ifndef _LOCAL_EVENTQUEUE_H
#define _LOCAL_EVENTQUEUE_H

#include "safe_circular_queue.h"
#include "event_type_list.h"
#include "enabled_modules.h"

#ifndef OS_EVENTQUEUE_LOCAL
typedef struct local_eventqueue
{
    safe_circular_queue_t internal_queue;
} local_eventqueue_t;

/**
 * @brief Initializes our local eventqueue
 * @param local_eventqueue_t *eventqueue pointer to eventqueue
 * @param event_data_t *data pointer to data

*/
int local_eventqueue_init(local_eventqueue_t *eventqueue, int max_events);

/**
 * @brief Enqueues an event to our local evenqueue
 * @param local_eventqueue_t *eventqueue pointer to eventqueue
 * @param event_data_t data to enqueue
 */
int local_eventqueue_enqueue(local_eventqueue_t *eventqueue, event_data_t data);

/**
 * @brief Checks how many events are inside the local eventqueue
 * @param local_eventqueue_t *eventqueue pointer to eventqueue
 * @param int *num events pointer to number to set to number of event
 */
int local_eventqueue_numevents(local_eventqueue_t *eventqueue, int *num_events);

/**
 * @brief Checks the top of the eventquueue, pulls down that data
 * @param local_eventqueue_t *eventqueue pointer to eventqueue

*/
int local_eventqueue_peektop(local_eventqueue_t *eventqueue, event_data_t *data);

/**
 * @brief Gets topmost element from the queue.
 * @param local_eventqueue_t *eventqueue pointer to eventqueue
 * @param event_data_t *data pointer to data
 */
int local_eventqueue_dequeue(local_eventqueue_t *eventqueue, event_data_t *data);
#endif
#endif