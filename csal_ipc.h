#ifndef _CSAL_IPC_H
#define _CSAL_IPC_H

#include "stdlib.h"
#include "stdint.h"

/**
 * @brief What is the largest buffer size we'd want our array to be
 */
#define BUFF_ARR_MAX_SIZE 4096

typedef enum ipc_message_type_enum
{
    IPC_MESSAGE_ERROR = 0,
    IPC_MESSAGE_ACK,
    IPCM_MESSAGE_NONE,
    IPC_MESSAGE_JSON,
    IPC_MESSAGE_BYTEARRAY,
} ipc_message_type_enum_t;

/**
 * @brief Message header before we get the actual JSON message so we known the length of the string
 */
#define IPC_MESSAGE_HANDLER_SIZE 9 * sizeof(uint8_t)
typedef struct ipc_message_header
{
    int32_t message_len;
    int32_t message_id;
    uint8_t message_type_enum;
} ipc_message_header_t;

/**
 * @brief Allows us to easily deserialize a message
 * @param uint8_t *buffer pointer to data we are deserializing
 * @param size_t size of buffer, make sure we aren't doing an overwrite
 * @return ipc_message_header_t returned message header for message unpacking
 * @note Will return a message_len of -1 if deserializating fails
 */
ipc_message_header_t deserialize_message_header(uint8_t *buffer, size_t len);

/**
 * @brief Allows us to easily serialize a message
 * @param ipc_message_header_t message we want to serialize
 * @param uint8_t *buffer pointer to the buffer we want to serialize into
 * @param size_t size of buffer we want to serialize into
 * @return bool whether or not we were able to serialize correctly
 */
bool serialize_message_header(ipc_message_header_t msg, uint8_t *buffer, size_t len);

#endif