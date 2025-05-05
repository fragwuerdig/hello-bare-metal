.globl switch_context
.type switch_context, @function
switch_context:
    // Arguments: rdi = &prev, rsi = &next

    // we are using rax as jump label for context switch_context
    // this is why we need to push it here. will be cleaned up
    // at label 1
    push %rax

    // Save callee-saved registers to prev
    movq %r15, 0x00(%rdi)
    movq %r14, 0x08(%rdi)
    movq %r13, 0x10(%rdi)
    movq %r12, 0x18(%rdi)
    movq %r11, 0x20(%rdi)
    movq %r10, 0x28(%rdi)
    movq %r9,  0x30(%rdi)
    movq %r8,  0x38(%rdi)
    movq %rbp, 0x50(%rdi)
    movq %rdx, 0x58(%rdi)
    movq %rcx, 0x60(%rdi)
    movq %rbx, 0x68(%rdi)
    movq %rsp, 0xb0(%rdi)
    
    // Save return address (rip) manually using label
    leaq 1f(%rip), %rax
    movq %rax, 0x98(%rdi)

    // Restore next context
    movq 0x00(%rsi), %r15
    movq 0x08(%rsi), %r14
    movq 0x10(%rsi), %r13
    movq 0x18(%rsi), %r12
    movq 0x20(%rsi), %r11
    movq 0x28(%rsi), %r10
    movq 0x30(%rsi), %r9
    movq 0x38(%rsi), %r8
    movq 0x50(%rsi), %rbp
    movq 0x58(%rsi), %rdx
    movq 0x60(%rsi), %rcx
    movq 0x68(%rsi), %rbx
    movq 0x98(%rsi), %rax
    movq 0xb0(%rsi), %rsp
    jmp *%rax

1: 
    pop %rax
    ret
