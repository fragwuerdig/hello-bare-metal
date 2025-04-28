#include <stdint.h>

#include "ringbuffer.h"
#include "paging.h"

static bool keyboard_putchar(uint8_t data);

volatile static ringbuffer_t keyboard_buffer_g;
static bool shift_pressed = false;

static const char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', /* 0x00 - 0x09 */
    '9', '0', '-', '=', '\b', '\t', /* Backspace + Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, /* Right shift */
    '*', 0, ' ', 0, /* Space bar, Caps Lock */
    /* Rest kann 0 bleiben */
};

static void keyboard_handler() {
    uint8_t scancode = inb(0x60); 

    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) {
            shift_pressed = false;
        }
    } else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = true;
        } else {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                if (shift_pressed && c >= 'a' && c <= 'z') {
                    c = c - 'a' + 'A';
                }
                keyboard_putchar(c);
            }
        }
    }

}

static bool keyboard_putchar(uint8_t data) {
    return ringbuffer_push(&keyboard_buffer_g, &data);
}

void keyboard_init() {
    keyboard_buffer_g.element_size = sizeof(uint8_t);
    keyboard_buffer_g.size = 256;
    keyboard_buffer_g.head = 0;
   
    keyboard_buffer_g.tail = 0;
   
    keyboard_buffer_g.buffer = early_alloc_continuous_pages(1);
    if (keyboard_buffer_g.buffer == 0) {
        // TODO: Handle memory allocation failure
    }
    install_irq_handler(1, (void*)keyboard_handler);
}

bool keyboard_getchar(uint8_t* out) {
    return ringbuffer_pop(&keyboard_buffer_g, out);
}

