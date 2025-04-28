#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;     // bits 0–15 of segment limit
    uint16_t base_low;      // bits 0–15 of base
    uint8_t  base_mid;      // bits 16–23 of base
    uint8_t  access;        // access flags
    uint8_t  granularity;   // granularity + high 4 bits of limit
    uint8_t  base_high;     // bits 24–31 of base
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;  // size of GDT - 1
    uint32_t base;   // address of first GDT entry
} __attribute__((packed));

#define GDT_ENTRY_COUNT 3

struct gdt_entry gdt[GDT_ENTRY_COUNT];
struct gdt_ptr gdt_descriptor;

void gdt_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].limit_low    = (limit & 0xFFFF);
    gdt[i].base_low     = (base & 0xFFFF);
    gdt[i].base_mid     = (base >> 16) & 0xFF;
    gdt[i].access       = access;
    gdt[i].granularity  = (gran & 0xF0) | ((limit >> 16) & 0x0F);
    gdt[i].base_high    = (base >> 24) & 0xFF;
}

void gdt_install() {
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint32_t)&gdt;

    __asm__ volatile ("lgdt (%0)" : : "r"(&gdt_descriptor));

    __asm__ volatile (
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "jmp $0x08, $.flush\n\t"
        ".flush:\n\t"
        : : : "memory", "ax"
    );
}
