#ifndef _BYTE_FIO_H
#define _BYTE_FIO_H

#include "stdint.h"
#include "os_mutx.h"
#include "os_setbits.h"
#include "platform_cshal.h"

/**
 * @brief Structure for a byte array FIFO (First-In, First-Out) buffer
 */
typedef struct {
    uint8_t *buffer; /**< Pointer to the buffer */
    int size; /**< Size of the buffer */
    int front; /**< Front index of the FIFO */
    int rear; /**< Rear index of the FIFO */
    int count; /**< Number of elements in the FIFO */
    os_mut_t mutex; /**< Mutex for thread safety */
    os_setbits_t block_til_data;
    int req_count;
    os_thread_id_t current_blocking_thread_handle;
    bool someone_blocking;
} byte_array_fifo;

/**
 * @brief Initializes a byte array FIFO.
 * @param size Size of the FIFO buffer.
 * @return Pointer to the created FIFO on success, NULL on failure.
 */
byte_array_fifo* create_byte_array_fifo(int size);

/**
 * @brief Destroys a byte array FIFO and frees memory.
 * @param fifo Pointer to the FIFO to be destroyed.
 */
int destroy_byte_array_fifo(byte_array_fifo* fifo);

/**
 * @brief Checks if the byte array FIFO is full.
 * @param fifo Pointer to the FIFO.
 * @return True if the FIFO is full, otherwise false.
 */
bool is_byte_array_fifo_full(byte_array_fifo* fifo);

/**
 * @brief Checks if the byte array FIFO is empty.
 * @param fifo Pointer to the FIFO.
 * @return True if the FIFO is empty, otherwise false.
 */
bool is_byte_array_fifo_empty(byte_array_fifo* fifo);

/**
 * @returns How much data is currently in the fifo
*/
int fifo_byte_array_count(byte_array_fifo* fifo);

/**
 * @brief Enqueues a byte into the byte array FIFO.
 * @param fifo Pointer to the FIFO.
 * @param data Data to be enqueued.
 * @return True if the enqueue operation is successful, otherwise false.
 */
int enqueue_byte_array_fifo(byte_array_fifo* fifo, uint8_t data);

/**
 * @brief empty and reset fifo
*/
int fifo_flush(byte_array_fifo* fifo);

/**
 * @brief Dequeues a byte from the byte array FIFO.
 * @param fifo Pointer to the FIFO.
 * @param data Pointer to store the dequeued data.
 * @return True if the dequeue operation is successful, otherwise false.
 */
int dequeue_byte_array_fifo(byte_array_fifo* fifo, uint8_t* data);

/**
 * @brief Try to dequeue as many bytes as possible(up to int len) from the fifo
 * @param fifo Pointer to the FIFO.
 * @param int len of data you want to dequeue
 * @param uint8_t *data Data to be enqueued.
 * @return OS_RET_OK if the enqueue operation is successful, otherwise false.
*/
int dequeue_bytes_bytearray_fifo(byte_array_fifo* fifo, uint8_t *data, int len);

/**
 * @brief Waits until we have x bytes in the fifo, blocking operation
 * @param fifo Pointer to the FIFO.
 * @param int bytes number of bytes to block on
*/
int block_until_n_bytes_fifo(byte_array_fifo* fifo, int bytes);

/**
 * @brief Tries to put whatever bytes u got into the fifo, will immediately fail out if it won't fit no more
 * @param int len of data you want to enqueue
 * @param uint8_t *data data to be enqueued.
 * @return OS_RET_OK if the enqueue operation is successful, otherwise fail.
*/
int enqueue_bytes_bytearray_fifo(byte_array_fifo* fifo, uint8_t *data, int len);
#endif