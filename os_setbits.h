#ifndef _OS_SETBITS_H
#define _OS_SETBITS_H

#include "stdlib.h"
#include "stdint.h"
#include "global_includes.h"

/**
 * @brief Initializes our setbits module
 * @param os_setbits_t *pointer to module
 */
int os_setbits_init(os_setbits_t *mod);

/**
 * @brief Signals a specific bit
 * @param os_setbits_t *pointer to module
 */
int os_setbits_signal(os_setbits_t *mod, uint8_t bit);

/**
 * @brief Clears a specific bit
 * @param os_setbits_t *pointer to module
 */
int os_clearbits(os_setbits_t *mod, uint8_t bit);

/**
 * @brief Wait for a specific bit to be set
 * @param os_setbits_t *pointer to module
 */
int os_waitbits(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms);

/**
 * @brief Wait indefinitely for a specific bit to be set
 * @param os_setbits_t *pointer to module
 */
int os_waitbits_indefinite(os_setbits_t *mod, uint8_t bit);

#endif