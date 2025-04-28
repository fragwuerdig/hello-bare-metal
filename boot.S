.global _start
.code32

_start:
    cli

    lgdt gdt64_descriptor

    # Enable Long Mode (LME bit in EFER MSR)
    mov $0xC0000080, %ecx
    rdmsr
    or $0x00000100, %eax
    wrmsr

    # Enable PAE
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    # Set CR3 to point to PML4
    mov $pml4_table, %eax
    mov %eax, %cr3

    # Enable paging
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    # Now far jump into 64-bit long mode
    ljmp $0x08, $long_mode_entry
    
.hang32:
    jmp .hang32


.code64

long_mode_entry:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    # setup early setup_identity_paging
    call setup_identity_paging

    # allocate stack memory and setup stack
    mov $4, %rdi
    call early_alloc_stack
    mov %rax, %rsp

    # main kernel entrypoint
    call kernel_main64

.hang:
    hlt
    jmp .hang

.align 4096
pml4_table:
    .set temp, pdpt_table + 0x03
    .quad temp

.align 4096
pdpt_table:
    .set temp, pd_table + 0x03
    .quad temp

.align 4096
pd_table:
    .set temp, 0x00000000 + 0x83
    .quad temp

.align 16
gdt64:
    .quad 0x0000000000000000          # Null descriptor
    .quad 0x00AF9A000000FFFF          # Code segment descriptor
    .quad 0x00AF92000000FFFF          # Data segment descriptor

gdt64_descriptor:
    .word (gdt64_descriptor_end - gdt64) - 1
    .long gdt64
    .long 0

gdt64_descriptor_end:

.section .bss
.align 16
stack_bottom:
    .skip 4096
stack_top:
