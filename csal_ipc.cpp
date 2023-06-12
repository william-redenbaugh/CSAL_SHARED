#include "csal_ipc.h"

ipc_message_header_t deserialize_message_header(uint8_t *buffer, size_t len)
{
    ipc_message_header_t msg;
    msg.message_len = -1;

    if (len != IPC_MESSAGE_HANDLER_SIZE)
    {
        return msg;
    }

    msg.message_len =
        ((buffer[0] & 0xFF) << 0) |
        ((buffer[1] & 0xFF) << 8) |
        ((buffer[2] & 0xFF) << 16) |
        ((int32_t)(buffer[3] & 0xFF) << 24);

    msg.message_id =
        ((buffer[4] & 0xFF) << 0) |
        ((buffer[5] & 0xFF) << 8) |
        ((buffer[6] & 0xFF) << 16) |
        ((int32_t)(buffer[7] & 0xFF) << 24);

    // What type of message are we sending/receiving
    msg.message_type_enum = buffer[8];

    return msg;
}

bool serialize_message_header(ipc_message_header_t msg, uint8_t *buffer, size_t len)
{
    buffer[0] = (uint8_t)msg.message_len & 0xFF;
    buffer[1] = (uint8_t)(msg.message_len >> 8) & 0xFF;
    buffer[2] = (uint8_t)(msg.message_len >> 16) & 0xFF;
    buffer[3] = (uint8_t)(msg.message_len >> 24) & 0xFF;

    buffer[4] = (uint8_t)msg.message_id;
    buffer[5] = (uint8_t)(msg.message_id >> 8) & 0xFF;
    buffer[6] = (uint8_t)(msg.message_id >> 16) & 0xFF;
    buffer[7] = (uint8_t)(msg.message_id >> 24) & 0xFF;

    buffer[8] = msg.message_type_enum;
    return true;
}
