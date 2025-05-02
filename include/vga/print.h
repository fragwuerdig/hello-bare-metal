#ifndef PRINT_H
#define PRINT_H

#include<stdint.h>
#include<vga/vga.h>

uint64_t vga_print(vga_buffer_t *vga, const char * buf, uint8_t size);

#endif