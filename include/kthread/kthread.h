#ifndef KTHREAD_H
#define KTHREAD_H

#include <stdint.h>

#define KTHREAD_PRIO_MAX 255

typedef enum {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_BLOCKED,
    THREAD_SLEEPING,
    THREAD_ZOMBIE
} kthread_status_t;

typedef struct cpu_context {
    // General Purpose Registers
    uint64_t r15;       // 0x00
    uint64_t r14;       // 0x08
    uint64_t r13;       // 0x10
    uint64_t r12;       // 0x18
    uint64_t r11;       // 0x20
    uint64_t r10;       // 0x28
    uint64_t r9;        // 0x30
    uint64_t r8;        // 0x38
    uint64_t rsi;       // 0x40
    uint64_t rdi;       // 0x48
    uint64_t rbp;       // 0x50
    uint64_t rdx;       // 0x58
    uint64_t rcx;       // 0x60
    uint64_t rbx;       // 0x68
    uint64_t rax;       // 0x70

    // Segment Register (usually only needed for ring changes)
    uint64_t ds;        // 0x78
    uint64_t es;        // 0x80
    uint64_t fs;        // 0x88
    uint64_t gs;        // 0x90

    // Stack and Instruction Pointers + Flags
    uint64_t rip;       // 0x98
    uint64_t cs;        // 0xa0
    uint64_t rflags;    // 0xa8
    uint64_t rsp;       // 0xb0
    uint64_t ss;        // 0xb8

} kthread_cpu_context_t;

typedef struct kthread {

    kthread_cpu_context_t context;

    uint64_t id;
    char name[32];

    kthread_status_t status;
    uint8_t priority;
    uint32_t time_slice;

    
    void* stack_base;
    uint64_t stack_size;

    // for later?
    // struct process* parent;

    // for later?
    // uint64_t page_table;;

    struct kthread* next;
    uint8_t __padding0;
    struct kthread* prev;
    uint8_t __padding1;
} kthread_t;

void kthread_init(
    kthread_t* t, const char * name,
    void (*entry)(void),
    uint8_t priority,
    void * stack_base,
    uint64_t stack_size
);
void kthread_init_cpu_queue(uint8_t cpu_id);
uint8_t kthread_get_cpu_id();

#endif