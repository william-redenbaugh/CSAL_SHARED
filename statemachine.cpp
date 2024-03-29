#include "statemachine.h"
#include "global_includes.h"
#ifdef STATEMACHINE

// #define STATEMACHINE_DEBUGGING
#ifdef STATEMACHINE_DEBUGGING
#define statemachine_println(e...)    \
    print("[STATEMACHINE MODULE]: "); \
    print(e);                         \
    print("\n")
#else
#define statemachine_println(e...) ((void)0)
#endif

statemachine_t *init_new_statemachine(const int num_states, const int init_state, statemachine_state_t *states_list)
{

    os_thread_sleep_s(1);
    // Basic bounds check
    if (num_states <= 0 || states_list == NULL)
        return NULL;

    statemachine_println("Allocating Statemachine dataa structures");
    statemachine_t *statemachine = (statemachine_t *)malloc(sizeof(statemachine_t));

    if (statemachine == NULL)
    {
        statemachine_println("Couldn't allocate memory");
    }

    os_thread_sleep_s(1);
    statemachine->current_state = init_state;
    statemachine->latest_event = 0;
    statemachine->num_states = num_states;
    statemachine->latest_event = NULL_EVENT;
    statemachine->states_list = states_list;

    os_thread_sleep_s(1);
    for (int n = 0; n < num_states; n++)
    {
        statemachine_println("Initialing state %d", n);

        os_thread_sleep_ms(100);
        // Init and clear all event submission data.
        for (int k = 0; k < statemachine->states_list[n].num_events; k++)
        {
            event_submission_t *event_sb = &statemachine->states_list[n].events_list[k];
            os_thread_sleep_ms(100);
            statemachine_println("Initialing event %d", k);
            if (event_sb == NULL)
            {
                statemachine_println("NULL pointer on event submission");
                free(statemachine);
                return NULL;
            }
        }
    }

    return statemachine;
}

int reinit_statemachine(statemachine_t *statemachine, const int num_states, const int init_state, statemachine_state_t *states_list)
{

    statemachine_println("Reinitializing Statemachine with new state and eventlist");
    // Basic bounds check
    if (num_states <= 0 || states_list == NULL)
        return OS_RET_INVALID_PARAM;
    statemachine->current_state = init_state;
    statemachine->latest_event = 0;
    statemachine->num_states = num_states;
    statemachine->latest_event = NULL_EVENT;
    statemachine->states_list = states_list;

    for (int n = 0; n < num_states; n++)
    {
        statemachine_println("Re initialization of state %d", n);
        // Init and clear all event submission data.
        for (int k = 0; k < statemachine->states_list[n].num_events; k++)
        {
            event_submission_t *event_sb = &statemachine->states_list[n].events_list[k];

            statemachine_println("Re initialization of event %d", k);
            if (event_sb == NULL)
            {
                statemachine_println("Null event in statemachine");
                return OS_RET_INT_ERR;
            }
        }
    }

    return OS_RET_OK;
}

int statemachine_submit_event(statemachine_t *statemachine, int event, void *params)
{
    if (statemachine == NULL)
    {
        return OS_RET_INT_ERR;
    }
    int current_state = statemachine->current_state;

    int next_state = -1;
    int event_index = -1;

    statemachine_println("Event submitted %d", event);
    statemachine_println("Current State: %d", statemachine->current_state);
    // Look through list of events, then add to state list
    for (int n = 0; n < statemachine->states_list[current_state].num_events; n++)
    {

        // Find correct event id, then submit!
        if (event == statemachine->states_list[current_state].events_list[n].event_id)
        {
            statemachine_println("Event found");
            // Set next state value
            next_state = statemachine->states_list[current_state].events_list[n].next_state;
            // Then next event index value
            event_index = statemachine->states_list[current_state].events_list[n].event_id;
            break;
        }

        if (END_EVENT == statemachine->states_list[current_state].events_list[n].event_id)
        {
            statemachine_println("Event not found");
            return OS_RET_NOT_INITIALIZED;
        }
    }
    // Couldn't find correct event in current statemachine
    // Return error and don't exit/enter states or run event callbacks
    if (next_state == -1)
        return OS_RET_INVALID_PARAM;

    // Run exit function
    if (statemachine->states_list[current_state].exit_function != NULL)
    {
        statemachine_println("Running exit function");

        statemachine->states_list[current_state].exit_function(
            event,
            current_state,
            &next_state,
            params);
    }

    // Run event callback
    if (statemachine->states_list[current_state].events_list[event_index].event_cb_function != NULL)
    {
        statemachine_println("Running event callback function");

        statemachine->states_list[current_state].events_list[event_index].event_cb_function(
            event,
            current_state,
            &next_state,
            params);
    }

    // Run entry function
    if (statemachine->states_list[next_state].entry_function != NULL)
    {
        statemachine_println("Running entry function");
        statemachine->states_list[next_state].entry_function(
            event,
            current_state,
            &next_state,
            params);
    }

    statemachine->current_state = next_state;

    return OS_RET_OK;
}

int statemachine_set_state(statemachine_t *statemachine, int next_state, void *param)
{
    if (statemachine == NULL || statemachine->num_events <= next_state)
        return OS_RET_INT_ERR;

    int current_state = statemachine->current_state;

    // Run exit function
    if (statemachine->states_list[current_state].exit_function != NULL)
        statemachine->states_list[current_state].exit_function(
            -1,
            current_state,
            &next_state,
            param);

    // Run entry function
    if (statemachine->states_list[next_state].entry_function != NULL)
    {
        statemachine->states_list[next_state].entry_function(
            -1,
            current_state,
            &next_state,
            param);
    }

    return OS_RET_OK;
}

int deconstruct_statemachine(statemachine_t *statemachine)
{
    if (statemachine == NULL)
    {
        return OS_RET_NULL_PTR;
    }

    statemachine_println("Deinitialize the statemachine");
    free(statemachine);

    return OS_RET_OK;
}
#endif