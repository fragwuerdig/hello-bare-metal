#include <stdint.h>
#include <gdt64.h>

struct gdt_entry gdt64[3];
struct gdt_ptr gdt64_descriptor;

void gdt64_install() {
    gdt64[0] = (struct gdt_entry){0}; // Null segment

    // Code Segment: base=0, limit=0, access=0x9A, flags=0x20 (long mode bit)
    gdt64[1] = (struct gdt_entry){
        .limit_low = 0x0000,
        .base_low = 0x0000,
        .base_mid = 0x00,
        .access = 0x9A,              // execute/read, accessed
        .granularity = 0x20,          // 64-bit segment
        .base_high = 0x00,
    };

    // Data Segment: regular data segment
    gdt64[2] = (struct gdt_entry){
        .limit_low = 0x0000,
        .base_low = 0x0000,
        .base_mid = 0x00,
        .access = 0x92,              // read/write, accessed
        .granularity = 0x00,
        .base_high = 0x00,
    };

    gdt64_descriptor.limit = sizeof(gdt64) - 1;
    gdt64_descriptor.base  = (uint64_t)&gdt64;
}
