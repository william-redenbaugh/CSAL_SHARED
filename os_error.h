#ifndef _OS_ERROR_H
#define _OS_ERROR_H

/**
 * @returns When the operation completed successfully
 */
#define OS_RET_OK (0)
#define OS_RET_OK_STR "OS_RET_OK"

/**
 * @brief When an internal error has occurred
 */
#define OS_RET_INT_ERR (-1)
#define OS_RET_INT_ERR_STR "OS_RET_INT_ERR"

/**
 * @returns When we have invalid parameters
 */
#define OS_RET_INVALID_PARAM (-2)
#define OS_RET_INVALID_PARAM_STR "OS_RET_INVALID_PARAM"

/**
 * @returns When there's an IO error during the operations
 */
#define OS_RET_IO_ERROR (-3)
#define OS_RET_IO_ERROR_STR "OS_RET_IO_ERROR"

/**
 * @returns No more memory available
 */
#define OS_RET_LOW_MEM_ERROR (-5)
#define OS_RET_LOW_MEM_ERROR_STR "OS_RET_LOW_MEM_ERROR"

/**
 * @returns When the system is not initialized
 */
#define OS_RET_NOT_INITIALIZED (-6)
#define OS_RET_NOT_INITIALIZED_STR "OS_RET_NOT_INITIALIZED"

/**
 * @returns When the list is empty!
 */
#define OS_RET_LIST_EMPTY (-7)
#define OS_RET_LIST_EMPTY_STR "OS_RET_LIST_EMPTY"

/**
 * @returns When there's some undefined error that we can't process
 */
#define OS_RET_UNDEFINED_ERROR (-8)
#define OS_RET_UNDEFINED_ERROR_STR "OS_RET_UNDEFINED_ERROR"

/**
 * @returns When there's a timeout during the operation
 */
#define OS_RET_TIMEOUT (-9)
#define OS_RET_TIMEOUT_STR "OS_RET_TIMEOUT"

/**
 * @returns When we have run out of reentrant locks
 */
#define OS_RET_MAX_RENTRANT (-10)
#define OS_RET_MAX_RENTRANT_STR "OS_RET_MAX_RENTRANT"

/**
 * @returns When we have a deadlock in our resource sharing
 */
#define OS_RET_DEADLOCK (-11)
#define OS_RET_DEADLOCK_STR "OS_RET_DEADLOCK"

/**
 * @returns When there are no more resources (outside of memory) left
 */
#define OS_RET_NO_MORE_RESOURCES (-12)
#define OS_RET_NO_MORE_RESOURCES_STR "OS_RET_NO_MORE_RESOURCES"

/**
 * @returns When we don't own the resources that we are trying to access
 */
#define OS_RET_NOT_OWNED (-13)
#define OS_RET_NOT_OWNED_STR "OS_RET_NOT_OWNED"

/**
 * @returns when we have an unsupported feature
 */
#define OS_RET_UNSUPPORTED_FEATURES (-14)
#define OS_RET_UNSUPPORTED_FEATURES_STR "OS_RET_UNSUPPORTED_FEATURES"

/**
 * @returns when the OS is null
 */
#define OS_RET_NULL_PTR (-15)
#define OS_RET_NULL_PTR_STR "OS_RET_NULL_PTR"

/**
 * @returns when the OS is already initialized this module
 */
#define OS_RET_ALREADY_INITED (-16)
#define OS_RET_ALREADY_INITED_STR "OS_RET_ALREADY_INITED"

/**
 * @returns when the OS has no available data
 */
#define OS_RET_NO_AVAILABLE_DATA (-17)
#define OS_RET_NO_AVAILABLE_DATA_STR "OS_RET_NO_AVAILABLE_DATA"

/**
 * @returns When OS failed to establish connection
 */
#define OS_RET_CONNECTION_FAILED (-18)
#define OS_RET_CONNECTION_FAILED_STR "OS_RET_CONNECTION_FAILED"

/**
 * @returns When OS failed to establish connection
 */
#define OS_RET_NO_FLASH_KEY (-18)
#define OS_RET_NO_FLASH_KEY_STR "OS_RET_NO_FLASH_KEY"

/**
 * @brief Converts POSIX Error messages to our OS Error messages
 */
int posix_os_ret_error_conv(int ret);

/**
 * @brief Let's us run an error if there's problem
 */
int os_panic(int ret);

#endif