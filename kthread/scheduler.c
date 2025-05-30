#include <kthread/kthread.h>
#include <kthread/scheduler.h>
#include <vga/vga.h>

kthread_per_cpu_queue_t PER_CPU_QUEUES[MAX_CPUS] = { 0 };
extern vga_buffer_t vga_buffer;

void kthread_enqueue(kthread_t* t, uint8_t cpu_id) {
    uint8_t prio = t->priority;

    t->status = THREAD_READY;
    t->next = PER_CPU_QUEUES[cpu_id].queues[prio];
    t->prev = 0;

    if (PER_CPU_QUEUES[cpu_id].queues[prio]) {
        PER_CPU_QUEUES[cpu_id].queues[prio]->prev = t;
    }

    PER_CPU_QUEUES[cpu_id].queues[prio] = t;
}

void kthread_scheduler_enqueue_tail(kthread_t* t, uint8_t cpu_id) {

    uint8_t prio = t->priority;
    
    t->status = THREAD_READY;

    t->prev = 0;
    t->next = 0;

    kthread_t* head = PER_CPU_QUEUES[cpu_id].queues[prio];
    
    if (!head) {
        PER_CPU_QUEUES[cpu_id].queues[prio] = t;
        return;
    }

    kthread_t* tail = head;
    while (tail->next)
        tail = tail->next;

    tail->next = t;
    t->prev = tail;

}


void kthread_dequeue(kthread_t* t, uint8_t cpu_id) {

    if (t->prev) { // t is not the head - unlink it
        t->prev->next = t->next;
    } else {   // t is the head - make next the new head
        PER_CPU_QUEUES[cpu_id].queues[t->priority] = t->next;
    }

    if (t->next)
        t->next->prev = t->prev;

    t->next = t->prev = 0;
}

static kthread_t* kthread_scheduler_pick(uint8_t cpu_id) {
    kthread_t* current = PER_CPU_QUEUES[cpu_id].current;
    //char buffer[256];
    for (int prio = KTHREAD_PRIO_MAX; prio >= 0; prio--) {
        kthread_t* t = PER_CPU_QUEUES[cpu_id].queues[prio];
        while (t) {
            if (t == current) {
                t = t->next;
                continue;
            }
            if (t->status == THREAD_READY) {
                return t;
            }
            t = t->next;
        }
    }

    // could not find a thread to run - pick current if its is READY
    if (current && current->status == THREAD_READY) {
        return current;
    }
    return 0;
}

static void kthread_switch_to(kthread_t* prev, kthread_t* next) {    
    switch_context((kthread_cpu_context_t*)prev, (kthread_cpu_context_t*)next);
}

void kthread_scheduler(uint8_t cpu_id) {
    
    // 1. Pick the next thread to run
    kthread_t* next = 0;
    next = kthread_scheduler_pick(cpu_id);

    // 2. Fallback: no ready thread found → run idle
    // TODO: Implement idle thread
    /*if (!next) {
        next = &idle_thread;
    }*/

    // 3. Avoid rescheduling the same thread unnecessarily
    if (next == PER_CPU_QUEUES[cpu_id].current) return;

    // 4. Update thread states
    if (PER_CPU_QUEUES[cpu_id].current && PER_CPU_QUEUES[cpu_id].current->status == THREAD_RUNNING)
        PER_CPU_QUEUES[cpu_id].current->status = THREAD_READY;

    next->status = THREAD_RUNNING;

    // 5. Perform context switch
    kthread_t* prev = PER_CPU_QUEUES[cpu_id].current;
    PER_CPU_QUEUES[cpu_id].current = next;

    if (!prev) {
        prev = next;
    }
    
    kthread_switch_to(prev, next);

}


// TODO: We are going to wrap cooperative multitasking
// in a software interrupt context switch, maybe some-
// thing from wich we can build some kind of "yield()"
// syscall later on.

// void kthread_yield(void) {
    
//     //char buffer[256];

//     //vga_snprintf(buffer, sizeof(buffer), "in kthread_yield\n");
//     //vga_print(&vga_buffer, buffer, sizeof(buffer));*/


//     uint8_t id = kthread_get_cpu_id();  // from per-cpu data or global

//     //vga_snprintf(buffer, sizeof(buffer), "kthread_yield current cpu id %d\n", id);
//     //vga_print(&vga_buffer, buffer, sizeof(buffer));*/

//     kthread_t* current = PER_CPU_QUEUES[id].current;
    
//     //vga_snprintf(buffer, sizeof(buffer), "kthread_yield current thread %x\n", current);
//     //vga_print(&vga_buffer, buffer, sizeof(buffer));*/

//     // Mark as READY and re-insert at end of run queue
//     current->status = THREAD_READY;
//     kthread_dequeue(current, id);  // dequeue current
//     kthread_scheduler_enqueue_tail(current, id);  // enqueue again

//     // Pick next thread and switch
//     kthread_scheduler(id);
// }