#include <stdint.h>
#include <gdt64.h>

struct gdt_entry gdt64[3];
struct gdt_ptr gdt64_descriptor;

static void gdt64_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt64[i].limit_low    = limit & 0xFFFF;
    gdt64[i].base_low     = base & 0xFFFF;
    gdt64[i].base_mid     = (base >> 16) & 0xFF;
    gdt64[i].access       = access;
    gdt64[i].granularity  = ((limit >> 16) & 0x0F) | (flags & 0xF0);
    gdt64[i].base_high    = (base >> 24) & 0xFF;
}

void gdt64_install() {
    gdt64[0] = (struct gdt_entry){0}; // Null segment

    gdt64_set_entry(1, 0, 0, 0x9A, 0x20); // code segment, L=1
    gdt64_set_entry(2, 0, 0, 0x92, 0x00); // data segment, L=0

    gdt64_descriptor.limit = sizeof(gdt64) - 1;
    gdt64_descriptor.base  = (uint64_t)&gdt64;

    __asm__ volatile ("lgdt (%0)" : : "r"(&gdt64_descriptor));

    gdt64_flush();
}
