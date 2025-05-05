#ifndef KTHREAD_SCHEDULER_H
#define KTHREAD_SCHEDULER_H

#include <kthread/kthread.h>

#define MAX_CPUS 4

typedef struct per_cpu_queue {
    uint8_t cpu_id;
    kthread_t* queues[KTHREAD_PRIO_MAX + 1];
    kthread_t* current;
} kthread_per_cpu_queue_t;

void kthread_enqueue(kthread_t* t, uint8_t cpu_id);
void kthread_scheduler_enqueue_tail(kthread_t* t, uint8_t cpu_id);
void kthread_dequeue(kthread_t* t, uint8_t cpu_id);
void kthread_scheduler(uint8_t cpu_id);

#endif