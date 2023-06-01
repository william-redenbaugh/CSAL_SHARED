#ifndef _OS_STATUS_H
#define _OS_STATUS_H

typedef enum {
    OS_STATUS_UNINITIALIZED = 0, 
    OS_STATUS_FAILED_INIT, 
    OS_STATUS_MODULE_FAILED,
    OS_STATUS_INITIALIZED
}os_status_t;

#endif