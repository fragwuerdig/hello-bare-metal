ENTRY(_start)
PAGING_BASE = 0x00100000;

SECTIONS {

    MULTIBOOT2 = 0x0010000;

    . = MULTIBOOT2;
    .multiboot2 : AT(MULTIBOOT2) {
        KEEP(*(.multiboot2))
    }

    .text : ALIGN(4K) {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }
}
