#ifndef MEM_LAYOUT_H
#define MEM_LAYOUT_H

#define MEM_SIZE_KERNEL_START          ( 0x80000000000ULL )
#define MEM_SIZE_DIRECT_PHYS_TO_VIRT   ( DIRECT_MAP_NUM_PAGES_1GiB * PAGE_SIZE_1GiB )
#define MEM_SIZE_KERNEL_HEAP           ( 0x80000000000ULL )

#define MEM_START_KERNEL_VIRT_SPACE    ( 0xFFFF800000000000ULL )
//#define MEM_START_DIRECT_PHYS_TO_VIRT  ( MEM_START_KERNEL_VIRT_SPACE + MEM_SIZE_KERNEL_START )
#define MEM_START_DIRECT_PHYS_TO_VIRT  ( 0xFFFF880000000000ULL )
//#define MEM_START_KERNEL_HEAP          ( MEM_START_DIRECT_PHYS_TO_VIRT + MEM_SIZE_DIRECT_PHYS_TO_VIRT )
#define MEM_START_KERNEL_HEAP          ( 0xFFFF880100000000ULL )

#define APIC_BASE       ( MEM_START_DIRECT_PHYS_TO_VIRT + 0xFEE00000ULL )
#define IOAPIC_BASE     ( MEM_START_DIRECT_PHYS_TO_VIRT + 0xFEC00000ULL )

#define IOREGSEL    *(volatile uint32_t*)(IOAPIC_BASE + 0x00)
#define IOWIN       *(volatile uint32_t*)(IOAPIC_BASE + 0x10)

#endif