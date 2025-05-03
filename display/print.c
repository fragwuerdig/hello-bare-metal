#include <stdint.h>
#include <vga/vga.h>

uint64_t vga_print(vga_buffer_t *vga_buffer, const char * buf, uint64_t size) {
    int i = 0;
    for (i = 0; i < size; i++) {
        if ( buf[i] == '\0' ) {
            return i+1;
        }
        vga_put_char(vga_buffer, buf[i]);
    }
    return i;
}