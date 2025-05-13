#include <stdint.h>
#include <mem/paging.h>
#include <vga/vga.h>
#include <vga/print.h>
#include <vga/format.h>
#include <kthread/kthread.h>
#include <kthread/scheduler.h>
#include <gdt64.h>
#include <mem/layout.h>

#define VGA_ADDRESS                 ( MEM_START_DIRECT_PHYS_TO_VIRT + 0xB8000ULL )
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void clear_screen() {
    volatile uint16_t *vga = (uint16_t *)VGA_ADDRESS;
    uint16_t blank = ' ' | (0xF0 << 8);

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = blank;
    }
}

void halt() {
    __asm__ volatile( "hlt" );
}

void crash_with_arg(uint64_t arg) {

    asm volatile (
        "mov %[input], %%rdi"
        :
        : [input] "r" (arg)
        : "rdi"
    );
    __asm__ volatile( "ud2" );
}

vga_buffer_t vga_buffer;

kthread_t thread1;
kthread_t thread2;
kthread_t idle_thread;

char buffer1[256];

void thread1_func() {
    
    uint64_t i = 0;

    while(1) {
        i=i%100;
        vga_snprintf(buffer1, sizeof(buffer1), "Hello World #1 %d\n", i++);
        vga_print(&vga_buffer, buffer1, sizeof(buffer1));
        //halt();
        //kthread_yield();
    }
    
    //halt();

}

void cli() {
    __asm__ volatile ("cli");
}

char buffer2[256];

void thread2_func() {
    
    uint64_t i = 0;

    while (1) {
        i=i%100;
        vga_snprintf(buffer2, sizeof(buffer2), "Hello World #2 %d\n", i++);
        vga_print(&vga_buffer, buffer2, sizeof(buffer2));
        //halt();
        //kthread_yield();
    }

    //halt();
    
}

void idle_thread_func() {

    static uint8_t initialized = 0;

    if (!initialized) {
        timer_init();
        initialized = 1;
    }

    while (1) {
        halt();
    }
}

void timer_handler() {
    char buffer[256];

    uint8_t id = kthread_get_cpu_id();
    
    *(volatile uint32_t *)(APIC_BASE + 0xB0ULL) = 0;

    kthread_scheduler(id);

}

void timer_init() {
    install_irq_handler(32, (void*)timer_handler);
    lapic_timer_init(0x40);
}

char buffer[256];

uint64_t kernel_main64() {

    cli();

    // setup direct mapping of physical memory to virtual memory
    mem_setup_direct_map_phys_to_virt();

    // relocate ourselves to the upper virtual address space
    jump_hi();

    // global and interrupt descriptor table setup
    gdt64_install();
    setup_idt();
    
    // initialize the keyboard handling
    keyboard_init();

    // init vga and clear the screen
    vga_init(&vga_buffer, (void *)VGA_ADDRESS, VGA_WIDTH, VGA_HEIGHT);
    clear_screen();

    // fist kernel message
    vga_snprintf(buffer, sizeof(buffer), "Hello World from kernel!\n");
    vga_print(&vga_buffer, buffer, sizeof(buffer));
    
    // initialize the cpu queues
    kthread_init_cpu_queue(0);

    // initialize threads
    void * stack1 = mem_alloc_stack_pages(32);
    void * stack2 = mem_alloc_stack_pages(32);
    void * stack3 = mem_alloc_stack_pages(32);    
    
    kthread_init(&thread1, "Thread 1", thread1_func, 2, stack1);
    kthread_init(&thread2, "Thread 2", thread2_func, 2, stack2);
    kthread_init(&idle_thread, "Idle Thread", idle_thread_func, 0, stack3);

    kthread_enqueue(&thread1, 0);
    kthread_enqueue(&thread2, 0);
    kthread_enqueue(&idle_thread, 0);

    //timer_init();

    // enter threading context
    kthread_enter(&idle_thread, 0);

    while (1)
    {
        halt();
    }
    
}
