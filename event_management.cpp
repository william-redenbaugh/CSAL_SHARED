#include "event_management.h"
#include "threads_list.h"
#include "os_error.h"

void event_management_init(void *params){
    
}

void event_management_thread(void *parameters){
    for(;;){

    }
}

int subscribe_eventlist(int *event_list, int num_events, int event_length_max){

    return OS_RET_OK;
}

bool available_events(int eventspace){

    return false;
}

event_data_t consume_event(int eventspace){
    event_data_t data;

    return data;
}