#include "os_error.h"
#include "global_includes.h"

int os_panic(int ret)
{
    if (ret == OS_RET_OK)
    {
        return 0;
    }

    switch (ret)
    {
    case OS_RET_INT_ERR:
        os_printf("Error: %s\n", OS_RET_INT_ERR_STR);
        break;

    case OS_RET_INVALID_PARAM:
        os_printf("Error: %s\n", OS_RET_INVALID_PARAM_STR);
        break;

    case OS_RET_IO_ERROR:
        os_printf("Error: %s\n", OS_RET_IO_ERROR_STR);
        break;

    case OS_RET_LOW_MEM_ERROR:
        os_printf("Error: %s\n", OS_RET_LOW_MEM_ERROR_STR);
        break;

    case OS_RET_NOT_INITIALIZED:
        os_printf("Error: %s\n", OS_RET_NOT_INITIALIZED_STR);
        break;

    case OS_RET_LIST_EMPTY:
        os_printf("Error: %s\n", OS_RET_LIST_EMPTY_STR);
        break;

    case OS_RET_UNDEFINED_ERROR:
        os_printf("Error: %s\n", OS_RET_UNDEFINED_ERROR_STR);
        break;

    case OS_RET_TIMEOUT:
        os_printf("Error: %s\n", OS_RET_TIMEOUT_STR);
        break;

    case OS_RET_MAX_RENTRANT:
        os_printf("Error: %s\n", OS_RET_MAX_RENTRANT_STR);
        break;

    case OS_RET_DEADLOCK:
        os_printf("Error: %s\n", OS_RET_DEADLOCK_STR);
        break;

    case OS_RET_NO_MORE_RESOURCES:
        os_printf("Error: %s\n", OS_RET_NO_MORE_RESOURCES_STR);
        break;

    case OS_RET_NOT_OWNED:
        os_printf("Error: %s\n", OS_RET_NOT_OWNED_STR);
        break;

    case OS_RET_UNSUPPORTED_FEATURES:
        os_printf("Error: %s\n", OS_RET_UNSUPPORTED_FEATURES_STR);
        break;

    case OS_RET_NULL_PTR:
        os_printf("Error: %s\n", OS_RET_NULL_PTR_STR);
        break;

    case OS_RET_ALREADY_INITED:
        os_printf("Error: %s\n", OS_RET_ALREADY_INITED_STR);
        break;

    case OS_RET_NO_AVAILABLE_DATA:
        os_printf("Error: %s\n", OS_RET_NO_AVAILABLE_DATA_STR);
        break;

    case OS_RET_NO_FLASH_KEY:
        os_printf("Errors: %s\n", OS_RET_NO_FLASH_KEY_STR);
        break;
    default:
        os_printf("Unknown Error: %d\n", ret);
        break;
    }

    os_thread_sleep_s(10);
    OS_ASSERT(ret != 0);
    // If you want to terminate the program when an error occurs, you can use exit(1) here.
    // exit(1);

    return ret;
}