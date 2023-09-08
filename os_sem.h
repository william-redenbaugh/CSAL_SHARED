#ifndef _SEM_H
#define _SEM_H

#include "global_includes.h"
/**
 * @brief Initializes a semaphore module
 * @param os_sem_t semaphore module
 */
int os_sem_init(os_sem_t *sem, int count);

/**
 * @brief Weak attempt at trying to grab a semaphore
 * @param os_sem_t
 */
int os_sem_try_entry(os_sem_t *sem);

/**
 * @brief Allows us to acquuire our semaphore
 * @param os_sem_t
 * @param uint32_t timeout for waiting
 */
int os_sem_entry(os_sem_t *sem, uint32_t timeout_ms);

/**
 * @brief How many re-entrants into the lock
 * @param os_sem_t
 */
int os_sem_count(os_sem_t *sem);

/**
 * @brief Waits indefinitely until the semaphore has a rentrant lock available
 * @param os_sem_t
 */
int os_sem_entry_wait_indefinite(os_sem_t *sem);

/**
 *  @brief Gives up count from semaphore
 * @param os_sem_t
 */
int os_sem_give(os_sem_t *sem);
#endif