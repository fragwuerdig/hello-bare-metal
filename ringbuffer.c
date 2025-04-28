#include "ringbuffer.h"
#include "paging.h"

void ringbuffer_init(ringbuffer_t* rb, uint8_t* backing_buffer, uint32_t element_size, uint32_t element_count) {
    rb->buffer = backing_buffer;
    rb->size = element_count;
    rb->element_size = element_size;
    rb->head = 0;
    rb->tail = 0;
}

bool ringbuffer_push(ringbuffer_t* rb, const void* data) {
    uint32_t next_head = (rb->head + 1) % rb->size;
    
    if (next_head == rb->tail) {
        // Buffer voll
        return false;
    }

    memcpy(rb->buffer + (rb->head * rb->element_size), data, rb->element_size);
    rb->head = next_head;
    return true;
}

bool ringbuffer_pop(ringbuffer_t* rb, void* out) {
    if (rb->head == rb->tail) {
        // Buffer leer
        return false;
    }

    memcpy(out, rb->buffer + (rb->tail * rb->element_size), rb->element_size);
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

bool ringbuffer_is_empty(ringbuffer_t* rb) {
    return rb->head == rb->tail;
}

bool ringbuffer_is_full(ringbuffer_t* rb) {
    return ((rb->head + 1) % rb->size) == rb->tail;
}
