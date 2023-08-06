#ifndef _LP_WORKQUEUE_H
#define _LP_WORKQUEUE_H

#include "platform_cshal.h"
#include "enabled_modules.h"

#ifdef OS_LP_WORKQUEUE_MOD

/**
 * @brief Function you want to add to the workqueeu
 */
typedef void (*wq_func)(void *param);

typedef struct lp_workqueue_func_node
{
    wq_func func;
    uint64_t interval_ms;
    uint64_t next_ms;
    void *param;
    struct lp_workqueue_func_node *next;

} lp_workqueue_func_node_t;

/**
 * @brief Primary low priority work queue
 */
typedef struct lp_workqueue
{
    lp_workqueue_func_node_t *head;
    os_mut_t wq_mtx;
    uint64_t shortest_interval_ms;
    int num_elements;
} lp_workqueue_t;

/**
 * @brief Initialization the work queue.
 * @param lp_workqueue_t *wq that we want to initialize
 * @param int max_elements_inqueue how many items can we have in the queue
 * @return os ret return status
 */
int _init_lp_workqueue(lp_workqueue_t *wq, int max_elements_inqueue);

/**
 * @brief Add a function to the workqueue
 * @param lp_workqueue_t* workqueue
 * @param wq_func function we want to add
 * @param int rough_scheduled_interval in milliseconds
 * @param lp_workqueue_func_node_t ** pointer to pointer for a node(should we want to remove from wq)
 */
int _lp_workqueue_add_func(lp_workqueue_t *wq, wq_func func, void *param, int interval_ms, lp_workqueue_func_node_t **ptr_node);

/**
 * @brief Add a function to the workqueue
 * @param wq_func function we want to add
 * @param int rough_scheduled_interval in milliseconds
 * @param lp_workqueue_func_node_t ** pointer to pointer for a node(should we want to remove from wq)
 */
int lp_workqueue_add_func(wq_func func, void *param, int interval_ms, lp_workqueue_func_node_t **ptr_node);

/**
 * @brief Removes a workqueue element from the low priority workqueue
 * @param lp_workqueue_t* workqueue
 * @param lp_workqueue_func_node_t *pointer to the workqueue node
 */
int _lp_workqueue_rm(lp_workqueue *wq, lp_workqueue_func_node_t *ptr_node);

/**
 * @brief Removes a workqueue element from the low priority workqueue
 * @param lp_workqueue_func_node_t *pointer to the workqueue node
 */
int lp_workqueue_rm(lp_workqueue_func_node_t *ptr_node);

/**
 * @brief Run this in the function you want to loop through your low priority function
 * @param lp_workqueue_t* workqueue
 */
int _lp_workqueue_loop(lp_workqueue_t *wq);

/**
 * @brief Initialization of the lpworkqueue thread
 */
void lp_workqueue_init(void *parameters);

/**
 * @brief If you are using the entire framework, this is the lpworkqueue thread
 */
void lp_workqueue_thread(void *parameters);

/**
 * @brief Tests out our low priority management thread
 */
void test_lprio(void);

#endif
#endif