#include <stdint.h>
#include "paging.h"
#include <vga/vga.h>
#include <vga/print.h>
#include <kthread/kthread.h>


#define MULTIBOOT2_HEADER_MAGIC      0xe85250D6
#define MULTIBOOT2_HEADER_ARCH       0
#define MULTIBOOT2_HEADER_LENGTH     16

#define VGA_ADDRESS                 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

__attribute__((section(".multiboot2"), aligned(8)))
const uint32_t multiboot2_header[] = {
    MULTIBOOT2_HEADER_MAGIC,
    MULTIBOOT2_HEADER_ARCH,
    MULTIBOOT2_HEADER_LENGTH,
    -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCH + MULTIBOOT2_HEADER_LENGTH),
};

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

void setup_early_stack(uint16_t num_stack_pages) {
    
    void* stack_base = early_alloc_continuous_pages(num_stack_pages);
    if (stack_base == 0) {
        // TODO: Handle allocation failure
        return;
    }

    uint64_t new_rsp = (uint64_t)stack_base + num_stack_pages * PAGE_SIZE;
    asm volatile (
        "mov %0, %%rsp\n"
        :
        : "r" (new_rsp)
        :
    );
}

vga_buffer_t vga_buffer;

kthread_t thread1;
kthread_t thread2;

void thread1_func() {
    
    uint8_t id = kthread_get_cpu_id();
    char buffer[256];

    for (int i = 0; i < 10; i++) {
        //vga_snprintf(buffer, sizeof(buffer), "Hello World #1 %d\n", i);
        //vga_print(&vga_buffer, buffer, sizeof(buffer));
        kthread_yield();
    }
    
    //halt();

}

void thread2_func() {

    uint8_t id = kthread_get_cpu_id();
    char buffer[256];

    for (int i = 0; i < 10; i++) {
        //vga_snprintf(buffer, sizeof(buffer), "Hello World #2 %d\n", i);
        //vga_print(&vga_buffer, buffer, sizeof(buffer));
        kthread_yield();
    }

    //halt();
    
}

void idle_thread_func() {
    while (1) {
        halt();
    }
}

uint64_t kernel_main64() {

    gdt64_install();
    setup_idt();
    clear_screen();
    keyboard_init();
    kthread_init_cpu_queue(0);

    vga_init(&vga_buffer, (void *)VGA_ADDRESS, VGA_WIDTH, VGA_HEIGHT);

    void * stack1 = early_alloc_stack(4);
    void * stack2 = early_alloc_stack(4);
    
    kthread_init(&thread1, "Thread 1", thread1_func, 3, stack1, 4*PAGE_SIZE);
    kthread_init(&thread2, "Thread 2", thread2_func, 2, stack2, 4*PAGE_SIZE);

    kthread_enqueue(&thread1, 0);
    kthread_enqueue(&thread2, 0);

    kthread_enter(&thread2);

    while (1)
    {
        halt();
    }
    
}
