#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_COLOR_WHITE_ON_BLACK 0x0F00
#define VGA_COLOR_BLACK_ON_WHITE 0xF000

#define VGA_BUFFER_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

typedef struct {
    void * buffer;
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t width;
    uint16_t height;
} vga_buffer_t;

void vga_init(vga_buffer_t *vga_buffer, void * buf, uint16_t width, uint16_t height);
void vga_set_pos(vga_buffer_t *vga_buffer, uint16_t x, uint16_t y);
void vga_put_char(vga_buffer_t *vga_buffer, char c);

#endif //VGA_H