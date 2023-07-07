#include "platform_cshal.h"

#include "lp_workqueue.h"
#include "os_error.h"
#include "stdlib.h"
#include "os_mutx.h"

int init_lp_workqueue(lp_workqueue_t *wq, int max_elements_inqueue){
    if(wq == NULL){
        return OS_RET_NULL_PTR;
    }

    wq->num_elements = max_elements_inqueue;
    return os_mut_init(&wq->wq_mtx);
}

int lp_workqueue_add_func(lp_workqueue_t *wq, wq_func func, void *param, int interval_ms, lp_workqueue_func_node_t **ptr_node){
    
    if(wq == NULL){
        return OS_RET_NULL_PTR;
    }
    lp_workqueue_func_node_t *node;

    int ret = os_mut_entry_wait_indefinite(&wq->wq_mtx);

    if(ret != OS_RET_OK){
        return ret;
    }

    if(wq->head == NULL){
        wq->head = (lp_workqueue_func_node_t*)malloc(sizeof(lp_workqueue_func_node_t));
        node = wq->head;
    }
    else{

        node =  wq->head;
        while (node->next != NULL)
        {
            node = node->next;
        }

        node->next = (lp_workqueue_func_node_t*)malloc(sizeof(lp_workqueue_func_node_t));
        node = node->next;
    }

    if(node == NULL){
        os_mut_exit(&wq->wq_mtx);
        return OS_RET_NO_MORE_RESOURCES;
    }
    
    node->interval_ms = interval_ms;
    node->next_ms = 0;
    node->func = func;
    node->param = param;
    node->next = NULL;

    if(ptr_node != NULL){
        *ptr_node = node;
    }
    
    ret = os_mut_exit(&wq->wq_mtx);

    return ret;
}

int lp_workqueue_rm(lp_workqueue *wq, lp_workqueue_func_node_t *ptr_node){

    int ret = os_mut_entry_wait_indefinite(&wq->wq_mtx);
    if(ret != OS_RET_OK){
        return ret;
    }
    lp_workqueue_func_node_t *node = wq->head;
    if(node == NULL){
        goto wq_rm_end;
    }

    while(node->next != NULL){
        // Delete then move on
        if(node->next == ptr_node){
            lp_workqueue_func_node_t *del_node = node->next;
            node->next = node->next->next;
            free(del_node);
        }
    }

    wq_rm_end: 
    ret = os_mut_exit(&wq->wq_mtx);
    return ret;
}

int lp_workqueue_loop(lp_workqueue_t *wq){
    if(wq == NULL){
        return OS_RET_NULL_PTR;
    }

    int ret = os_mut_entry_wait_indefinite(&wq->wq_mtx);
    lp_workqueue_func_node_t *node = wq->head;
    ret |= os_mut_exit(&wq->wq_mtx);

    if(ret != OS_RET_OK){
        return ret; 
    }
    
    while (true)
    {
        
        // Fill in parameters in threadsafe way
        os_mut_entry_wait_indefinite(&wq->wq_mtx);
        uint64_t next_ms = node->next_ms;
        wq_func func = node->func;
        void *param = node->param;
        os_mut_exit(&wq->wq_mtx);
        /**/
        // If we are ready to run next command
        if(next_ms < get_current_time_millis()){
            // run cb 
            func(param);

            // Update the next entry in threadsafe way
            os_mut_entry_wait_indefinite(&wq->wq_mtx);
            node->next_ms = get_current_time_millis() + node->interval_ms;
            os_mut_exit(&wq->wq_mtx);            
        }

        // Migrate to next node
        os_mut_entry_wait_indefinite(&wq->wq_mtx);
        node = node->next;
        os_mut_exit(&wq->wq_mtx);
        
        if(node == NULL)
            break;
    }

    return OS_RET_NULL_PTR;
}

#ifdef LP_WORKQUEUE_TESTS

lp_workqueue_t test_wq;
void lowprio_thread(void *params){
    for(;;){
        lp_workqueue_loop(&test_wq);
        os_thread_sleep_ms(1);
    }
}

void loprio_1s_cb(void *params){
    println("1 second callback");
}

void loprio_5s_cb(void *params){
    println("5 second callback");
}

void loprio_10s_cb(void *params){
    println("10 second callback");
}

void loprio_100ms_cb(void *params){
    println("100 millisecond callback");
}

void test_lprio(void){
    init_lp_workqueue(&test_wq, 40);
    lp_workqueue_add_func(&test_wq, loprio_1s_cb, NULL, 1000, NULL);
    lp_workqueue_add_func(&test_wq, loprio_5s_cb, NULL, 5000, NULL);
    lp_workqueue_add_func(&test_wq, loprio_10s_cb, NULL, 10000, NULL);
    lp_workqueue_add_func(&test_wq, loprio_100ms_cb, NULL, 100, NULL);
    
    os_add_thread(lowprio_thread, NULL, 8192, NULL);
}
#endif