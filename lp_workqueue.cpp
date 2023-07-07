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

int lp_workqueue_add_func(lp_workqueue_t *wq, wq_func func, void *param, int interval_ms){
    
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

        if(node == NULL){
            return OS_RET_NO_MORE_RESOURCES;
        }
    }
    else{

        node =  wq->head;
        while (node->next == NULL)
        {
            node = node->next;
        }

        node->next = (lp_workqueue_func_node_t*)malloc(sizeof(lp_workqueue_func_node_t));
    }

    node->interval_ms = interval_ms;
    node->next_ms = 0;
    node->func = func;
    node->param = param;
    node->next = NULL;

    ret = os_mut_exit(&wq->wq_mtx);

    if(ret != OS_RET_OK){
        return ret;
    }
    
}

int lp_workqueue_loop(lp_workqueue_t *wq){
    if(wq == NULL){
        return OS_RET_NULL_PTR;
    }

    int ret = os_mut_entry_wait_indefinite(&wq->wq_mtx);
    lp_workqueue_func_node_t *node;
    ret |= os_mut_exit(&wq->wq_mtx);

    if(ret != OS_RET_OK){
        return ret; 
    }

    while (true)
    {
        os_mut_entry_wait_indefinite(&wq->wq_mtx);
        uint64_t next_ms = node->next_ms;
        wq_func func = node->func;
        void *param = node->param;
        os_mut_exit(&wq->wq_mtx);

        if(next_ms < get_current_time_millis()){ 
            func(param);

            os_mut_entry_wait_indefinite(&wq->wq_mtx);
            node->next_ms = get_current_time_millis() + node->interval_ms;
            os_mut_exit(&wq->wq_mtx);            
        }

        os_mut_entry_wait_indefinite(&wq->wq_mtx);
        node = node->next;
        
        if(node == NULL)
            break;

        os_mut_exit(&wq->wq_mtx);
    }
}