#include <kthread/kthread.h>
#include <klib/string.h>
#include <kthread/scheduler.h>
#include <vga/vga.h>

extern kthread_per_cpu_queue_t PER_CPU_QUEUES[MAX_CPUS];

void kthread_init(
    kthread_t* t, const char * name,
    void (*entry)(void),
    uint8_t priority,
    void * stack_base,
    uint64_t stack_size
) {

    uint64_t name_len = strlen(name) > sizeof(t->name) ? sizeof(t->name) : strlen(name);
    memset(t, 0, sizeof(kthread_t));
    memcpy(t->name, name, name_len);
    t->priority = priority;
    t->status = THREAD_READY;
    t->context.rip = (uint64_t)entry;
    t->context.rsp = (uint64_t)stack_base;
    t->stack_base = stack_base;
    t->stack_size = stack_size;

}

void kthread_enter(kthread_t* t, uint8_t cpu_id) {
    t->status = THREAD_RUNNING;
    PER_CPU_QUEUES[cpu_id].current = t;
    kthread_enter_asm((kthread_cpu_context_t*)t);
}

#define IA32_GS_BASE_MSR 0xC0000101

void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low  = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile (
        "wrmsr" : : "c"(msr), "a"(low), "d"(high) : "memory"
    );
}

void rdmsr(uint32_t msr, uint64_t *value) {
    uint32_t low, high;
    asm volatile (
        "rdmsr" : "=a"(low), "=d"(high) : "c"(msr)
    );
    *value = ((uint64_t)high << 32) | low;
}

void kthread_init_cpu_queue(uint8_t cpu_id) {
    PER_CPU_QUEUES[cpu_id].cpu_id = cpu_id;
    wrmsr(IA32_GS_BASE_MSR, (uint64_t)&PER_CPU_QUEUES[cpu_id]);
}

uint8_t kthread_get_cpu_id() {
    uint64_t value;
    rdmsr(IA32_GS_BASE_MSR, &value);
    return (uint8_t)((kthread_per_cpu_queue_t *)value)->cpu_id;
}