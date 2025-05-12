#include <stdint.h>
#include <mem/paging.h>
#include <mem/layout.h>
#include "irq.h"
#include "isr.h"

#define IA32_APIC_BASE_MSR 0x1B


typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} idt_entry_t ;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idt_descriptor_t;

typedef struct {
    idt_entry_t entries[256];
    idt_descriptor_t descriptor; 
} idt_struct_t;

void idt_set_entry(idt_entry_t* idt, int vector, void* isr, uint16_t selector, uint8_t type_attr) { 
    uint64_t isr_addr = (uint64_t)isr;
    idt[vector].offset_low  = isr_addr & 0xFFFF;
    idt[vector].selector    = selector;
    idt[vector].ist         = 0; // no IST used for now
    idt[vector].type_attr   = type_attr;
    idt[vector].offset_mid  = (isr_addr >> 16) & 0xFFFF;
    idt[vector].offset_high = (isr_addr >> 32) & 0xFFFFFFFF;
    idt[vector].zero        = 0;
}

void empty_handler() {
    // This is a placeholder for unhandled interrupts
    // You can add logging or debugging code here
}

void setup_idt() {
    
    init_irq_descriptor();
    
    pic_remap();
    //pic_disable();

    volatile idt_struct_t * idt_struct = mem_alloc_pages((sizeof(idt_struct_t) + PAGE_SIZE_4KiB - 1) / PAGE_SIZE_4KiB);
    if (idt_struct == 0) {
        // TODO: Handle error
        return;
    }

    idt_set_entry(idt_struct->entries, 0x20, irq0_handler, 0x08, 0x8E);
    idt_set_entry(idt_struct->entries, 0x21, irq1_handler, 0x08, 0x8E);
    idt_set_entry(idt_struct->entries, 0x40, irq32_handler, 0x08, 0x8E);
    
    install_irq_handler(0, (void*)empty_handler);

    idt_struct->descriptor.limit = sizeof(idt_struct->entries) - 1;
    idt_struct->descriptor.base  = ((uint64_t)idt_struct->entries);

    load_idt(&idt_struct->descriptor);
    __asm__ volatile ("sti");
}

void lapic_enable() {
    uint64_t apic_base = rdmsr(IA32_APIC_BASE_MSR);
    apic_base |= (1 << 11); // bit 11 = APIC global enable
    wrmsr(IA32_APIC_BASE_MSR, apic_base);
    volatile uint32_t* lapic_svr = (uint32_t*)(APIC_BASE + 0xF0);
    *lapic_svr |= 0x100; // enable APIC via Spurious Interrupt Vector register
}

void lapic_timer_init(uint8_t vector) {
    volatile uint32_t* divide = (uint32_t*)(APIC_BASE + 0x3E0);
    volatile uint32_t* lvt_timer = (uint32_t*)(APIC_BASE + 0x320);
    volatile uint32_t* init_count = (uint32_t*)(APIC_BASE + 0x380);

    *divide = 0b1011;            // divide by 128
    *lvt_timer = vector | (1 << 17); // vector + periodic mode
    *init_count = 781250000;       // initial count (experimentell)
}

uint32_t ioapic_read(uint8_t reg) {
    IOREGSEL = reg;
    return IOWIN;
}

void ioapic_write(uint8_t reg, uint32_t value) {
    IOREGSEL = reg;
    IOWIN = value;
}

void ioapic_set_entry(int irq, uint8_t vector, uint8_t apic_id) {
    uint32_t low = vector; // z. B. 0x21
    low |= (0 << 16); // delivery mode: fixed
    low |= (0 << 11); // dest mode: physical
    low |= (0 << 13); // polarity: high active
    low |= (0 << 15); // trigger: edge
    low |= (0 << 16); // mask: 0 = enabled

    uint32_t high = apic_id << 24; // Destination APIC ID

    ioapic_write(0x10 + 2 * irq, low);
    ioapic_write(0x10 + 2 * irq + 1, high);
}

void pic_disable() {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void pic_remap() {
    // PIC ports
    const uint8_t PIC1_COMMAND = 0x20;
    const uint8_t PIC1_DATA    = 0x21;
    const uint8_t PIC2_COMMAND = 0xA0;
    const uint8_t PIC2_DATA    = 0xA1;

    const uint8_t ICW1_INIT = 0x10;
    const uint8_t ICW1_ICW4 = 0x01;
    const uint8_t ICW4_8086 = 0x01;

    // Start initialization sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // Set vector offset
    outb(PIC1_DATA, 0x20); // Master PIC vectors start at 0x20
    outb(PIC2_DATA, 0x28); // Slave PIC vectors start at 0x28

    // Tell Master PIC there is a slave PIC at IRQ2
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // Set 8086/88 mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Unmask IRQ1 (keyboard), mask all others
    outb(PIC1_DATA, 0xFD); // 11111101 - only IRQ1 allowed
    outb(PIC2_DATA, 0xFF); // Mask all IRQs on slave PIC
}
