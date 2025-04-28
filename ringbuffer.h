#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t* buffer;
    uint32_t size;          // Anzahl Elemente
    uint32_t element_size;  // Größe eines Elements
    uint32_t head;
    uint32_t tail;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t* rb, uint8_t* backing_buffer, uint32_t element_size, uint32_t element_count);
bool ringbuffer_push(ringbuffer_t* rb, const void* data);
bool ringbuffer_pop(ringbuffer_t* rb, void* out);
bool ringbuffer_is_empty(ringbuffer_t* rb);
bool ringbuffer_is_full(ringbuffer_t* rb);

#endif
