
#include <vga/vga.h>
#include <stdint.h>

static uint16_t offset(vga_buffer_t *vga_buffer) {
    return vga_buffer->pos_y * vga_buffer->width + vga_buffer->pos_x;
}

static void clear_last_row(vga_buffer_t *vga_buffer) {
    uint16_t *buffer = (uint16_t *)vga_buffer->buffer;
    for (uint16_t col = 0; col < vga_buffer->width; col++) {
        buffer[(vga_buffer->height - 1) * vga_buffer->width + col] = (uint16_t)' ' | VGA_COLOR_BLACK_ON_WHITE;
    }
}

static void shift_up(vga_buffer_t *vga_buffer) {
    uint16_t * buffer = vga_buffer->buffer;
    for (uint16_t row = 1; row < vga_buffer->height; row++) {
        for (uint16_t col = 0; col < vga_buffer->width; col++) {
            buffer[(row - 1) * vga_buffer->width + col] = buffer[row * vga_buffer->width + col];
        }
    }   
}

// init the vga_buffer_t
void vga_init(vga_buffer_t *vga_buffer, void * buf, uint16_t width, uint16_t height) {
    vga_buffer->buffer = buf;
    vga_buffer->pos_x = 0;
    vga_buffer->pos_y = 0;
    vga_buffer->width = width;
    vga_buffer->height = height;
}

// set the cursor position of the terminal
void vga_set_pos(vga_buffer_t *vga_buffer, uint16_t x, uint16_t y) {
    if (x < vga_buffer->width && y < vga_buffer->height) {
        vga_buffer->pos_x = x;
        vga_buffer->pos_y = y;
    }
}

// puts a single char and advances the cursor
void vga_put_char(vga_buffer_t *vga_buffer, char c) {
    
    uint16_t *buffer = (uint16_t *)vga_buffer->buffer;

    if (c == '\n') {
        if (vga_buffer->pos_y >= vga_buffer->height - 1) {
            shift_up(vga_buffer);
            clear_last_row(vga_buffer);
            vga_set_pos(vga_buffer, 0, vga_buffer->height - 1);
        } else {
            vga_set_pos(vga_buffer, 0, vga_buffer->pos_y + 1);
        }
        return;
    }

    if (vga_buffer->pos_x >= vga_buffer->width-1) {
        if (vga_buffer->pos_y >= vga_buffer->height-1) {
            buffer[offset(vga_buffer)] = (uint16_t)c | VGA_COLOR_BLACK_ON_WHITE;
            shift_up(vga_buffer);
            clear_last_row(vga_buffer);
            vga_set_pos(vga_buffer, 0, vga_buffer->height - 1);
        } else {
            buffer[offset(vga_buffer)] = (uint16_t)c | VGA_COLOR_BLACK_ON_WHITE;
            vga_set_pos(vga_buffer, 0, vga_buffer->pos_y+1);
        }
        return;
    }

    buffer[offset(vga_buffer)] = (uint16_t)c | VGA_COLOR_BLACK_ON_WHITE;
    vga_set_pos(vga_buffer, vga_buffer->pos_x+1, vga_buffer->pos_y);

}