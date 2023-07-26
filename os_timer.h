/**
 * @file os_timer.h
 *
 * @brief Header file for the OS timer module.
 */

#ifndef OS_TIMER_H
#define OS_TIMER_H
#include "platform_cshal.h"
#include "stdint.h"

/**
 * @brief Initialize an OS timer.
 *
 * @param timer Pointer to the OS timer object to be initialized.
 * @param cb Callback function to be called when the timer expires.
 * @param interval_ms The interval in milliseconds between timer expirations.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_init(os_timer_t *timer, os_timer_cb_t cb, int interval_ms);

/**
 * @brief Start an OS timer in one-shot mode.
 *
 * The timer will expire once after the specified interval and then stop.
 *
 * @param timer Pointer to the initialized OS timer object.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_start_oneshot(os_timer_t *timer);

/**
 * @brief Start an OS timer in recurring mode.
 *
 * The timer will expire repeatedly at the specified interval until stopped.
 *
 * @param timer Pointer to the initialized OS timer object.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_start_recurring(os_timer_t *timer);

/**
 * @brief Change the interval of an active OS timer.
 *
 * This function can be used to modify the interval of an already active timer.
 *
 * @param interval_ms The new interval in milliseconds between timer expirations.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_interval_change(os_timer_t *timer, int interval_ms);

/**
 * @brief Stop an active OS timer.
 *
 * @param timer Pointer to the initialized OS timer object to be stopped.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_stop(os_timer_t *timer);

/**
 * @brief Deinitialize an OS timer and release associated resources.
 *
 * @param timer Pointer to the initialized OS timer object to be deinitialized.
 *
 * @return OS_RET_OK on success, an OS_RET error code on failure.
 */
int os_timer_deinit(os_timer_t *timer);

#endif /* OS_TIMER_H */