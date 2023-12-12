#include "byte_fifo.h"
#include "global_includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

byte_array_fifo* create_byte_array_fifo(int size) {
    byte_array_fifo* fifo = (byte_array_fifo*)malloc(sizeof(byte_array_fifo));
    if (fifo == NULL) {
        return NULL; // Unable to allocate memory for FIFO
    }

    fifo->buffer = (uint8_t*)malloc(size * sizeof(uint8_t));
    if (fifo->buffer == NULL) {
        free(fifo);
        return NULL; // Unable to allocate memory for buffer
    }

    fifo->size = size;
    fifo->front = 0;
    fifo->rear = 0;
    fifo->count = 0;
    fifo->req_count = 0;
    fifo->someone_blocking = false;

    // Initialize mutex
    if (os_mut_init(&fifo->mutex) != OS_RET_OK) {
        free(fifo->buffer);
        free(fifo);
        return NULL; // Unable to initialize mutex
    }
    
    if(os_setbits_init(&fifo->block_til_data) != OS_RET_OK){
        os_mut_deinit(&fifo->mutex);
        free(fifo->buffer);
        free(fifo);
        return NULL; // Unable to initialize mutex
    }

    return fifo;
}

int destroy_byte_array_fifo(byte_array_fifo* fifo) {
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    

    if (fifo != NULL) {
        // Deconstruct the concurrency primitives
        os_mut_deinit(&fifo->mutex);
        os_setbits_deconstruct(&fifo->block_til_data);

        // Free ze memory
        free(fifo->buffer);
        free(fifo);
    }

    return OS_RET_OK;
}

bool is_byte_array_fifo_full(byte_array_fifo* fifo) {
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    
    int ret = os_mut_entry(&fifo->mutex, 0);

    if(ret != OS_RET_OK){
        return ret;
    }

    bool result = (fifo->count == fifo->size);

    ret = os_mut_exit(&fifo->mutex);
    if(ret != OS_RET_OK){
        return ret;
    }
    return result;
}

bool is_byte_array_fifo_empty(byte_array_fifo* fifo) {
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    int ret =os_mut_entry(&fifo->mutex, 0);
    if(ret != OS_RET_OK){
        return ret;
    }

    bool result = (fifo->count == 0);

    ret = os_mut_exit(&fifo->mutex);
    if(ret != OS_RET_OK){
        return ret;
    }
    return result;
}

int enqueue_byte_array_fifo(byte_array_fifo* fifo, uint8_t data) {
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    
    int ret = os_mut_entry(&fifo->mutex, 0);
    if(ret != OS_RET_OK){
        return ret;
    }
    
    if (is_byte_array_fifo_full(fifo)) {
        os_mut_exit(&fifo->mutex);
        return OS_RET_NO_MORE_RESOURCES; // FIFO is full, cannot enqueue
    }

    fifo->buffer[fifo->rear] = data;
    fifo->rear = (fifo->rear + 1) % fifo->size;
    fifo->count++;

    if(fifo->req_count <= fifo->count){
        ret = os_setbits_signal(&fifo->block_til_data, BIT0);
        if(ret != OS_RET_OK){
            os_mut_exit(&fifo->mutex);
            return ret;
        }
    }

    ret = os_mut_exit(&fifo->mutex);
    if (is_byte_array_fifo_full(fifo)) {
        return false; // FIFO is full, cannot enqueue
    }

    return OS_RET_OK;
}

int enqueue_bytes_bytearray_fifo(byte_array_fifo* fifo, uint8_t *data, int len){
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    
    int ret = os_mut_entry(&fifo->mutex, 0);
    if (ret != OS_RET_OK) {
        return ret;
    }

    // No space heh
    if(fifo->size - fifo->count < len){
        ret = os_mut_exit(&fifo->mutex);
        if (ret != OS_RET_OK) {
            return ret;
        }

        return OS_RET_NO_MORE_RESOURCES;
    }

    for(int n = 0; n < len; n++){
        fifo->buffer[fifo->rear] = data[n];
        fifo->rear = (fifo->rear + 1) % fifo->size;
        fifo->count++;
    }

    if(fifo->req_count <= fifo->count){
        ret = os_setbits_signal(&fifo->block_til_data, BIT0);
        if (ret != OS_RET_OK) {
            return ret;
        }
    }

    ret = os_mut_exit(&fifo->mutex);
    return ret;
}

int dequeue_bytes_bytearray_fifo(byte_array_fifo* fifo, uint8_t *data, int len){
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }

    int ret = os_mut_entry(&fifo->mutex, 0);
    if (ret != OS_RET_OK) {
        return ret;
    }
    // Can only dequeue as many bytes as there are in the buffer hehe
    if(len > fifo->count)
        len = fifo->count;

    // WEEEEEEEEEEEEEEEEEE
    for(int n = 0; n < len; n++){
        data[n] = fifo->buffer[fifo->front];
        fifo->front = (fifo->front + 1) % fifo->size;
        fifo->count--;
    }

    // Signal if we got enough data for whoever
    if(fifo->req_count <= fifo->count){
        ret = os_setbits_signal(&fifo->block_til_data, BIT0);
        if (ret != OS_RET_OK) {
            return ret;
        }
    }

    ret = os_mut_exit(&fifo->mutex);
    if(ret != OS_RET_OK){
        return ret;
    }
    return len;
}

int dequeue_byte_array_fifo(byte_array_fifo* fifo, uint8_t* data) {
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }
    int ret = os_mut_entry(&fifo->mutex, 0);
    if (ret != OS_RET_OK) {
        return ret;
    }
    
    if (is_byte_array_fifo_empty(fifo)) {
        ret = os_mut_exit(&fifo->mutex);
        if (ret != OS_RET_OK) {
            return ret;
        }
        return OS_RET_NO_AVAILABLE_DATA;
    }

    *data = fifo->buffer[fifo->front];
    fifo->front = (fifo->front + 1) % fifo->size;
    fifo->count--;

    return os_mut_exit(&fifo->mutex);
}

int block_until_n_bytes_fifo(byte_array_fifo* fifo, int bytes){
    
    if(fifo == NULL){
        return OS_RET_NULL_PTR;
    }

    if(fifo->count >= bytes){
        return OS_RET_OK;
    }

    if(bytes > fifo->size){
        return OS_RET_INVALID_PARAM;
    }

    if((fifo->someone_blocking == true) && (os_cmp_id(os_current_id(), fifo->current_blocking_thread_handle))){
        return OS_RET_NOT_OWNED;
    }

    int ret = os_mut_entry(&fifo->mutex, 0);
    
    if(ret != OS_RET_OK){
        return ret;
    }

    // Set correct state
    fifo->someone_blocking = true;
    fifo->current_blocking_thread_handle = os_current_id();
    fifo->req_count = bytes;
    ret = os_clearbits(&fifo->block_til_data, BIT0);

    if(ret != OS_RET_OK){
        os_mut_exit(&fifo->mutex);
        return ret;
    }

    ret = os_mut_exit(&fifo->mutex);

    if(ret != OS_RET_OK){
        return ret;
    }

    // Block
    ret = os_waitbits_indefinite(&fifo->block_til_data, BIT0);

    if(ret != OS_RET_OK){
        return ret;
    }
    
    // Cleanup
    ret = os_clearbits(&fifo->block_til_data, BIT0);
    
    if(ret != OS_RET_OK){
        return ret;
    }

    // Clear someone blocking false
    ret = os_mut_entry(&fifo->mutex, 0);
    if(ret != OS_RET_OK){
        return ret;
    }
    fifo->someone_blocking = false;
    ret = os_mut_exit(&fifo->mutex);
    if(ret != OS_RET_OK){
        return ret;
    }
    return OS_RET_OK;
}

int fifo_flush(byte_array_fifo* fifo){
    // Clear someone blocking false
    int ret = os_mut_entry(&fifo->mutex, 0);
    if(ret != OS_RET_OK){
        return ret;
    }
    fifo->front = 0;
    fifo->rear = 0;
    fifo->count = 0;
    ret = os_mut_exit(&fifo->mutex);
    if(ret != OS_RET_OK){
        return ret;
    }

    return OS_RET_OK;
}