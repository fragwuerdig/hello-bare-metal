.globl kthread_enter_asm
.type kthread_enter_asm, @function
kthread_enter_asm:
    // Arguments: rdi = &cpu_ctx
    movq 0xb0(%rdi), %rsp   // Load stack pointer (context.rsp)
    movq 0x98(%rdi), %rax   // Load instruction pointer (context.rip)
    jmp *%rax               // Start executing thread