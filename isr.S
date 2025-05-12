.global isr_common_stub
.global irq_handler_table
.global load_idt
.global eoi

.macro IRQ_HANDLER num
.global irq\num\()_handler
irq\num\()_handler:
    cli
    push $\num
    jmp irq_common_entry 
.endm

irq_common_entry:
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    
    movq 120(%rsp),  %rdi
    call irq_dispatcher

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
    
    // clean up stack value
    // pushed by entry-point
    add $8, %rsp

eoi:
    movb $0x20, %al
    outb %al, $0x20
    iretq

load_idt:
    movq %rdi, %rax
    lidt (%rax)
    ret

IRQ_HANDLER 0
IRQ_HANDLER 1
IRQ_HANDLER 32
