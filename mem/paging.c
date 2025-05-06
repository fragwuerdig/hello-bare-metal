#include <stdint.h>
#include <mem/paging.h>
#include <mem/layout.h>
#include "klib/string.h"

// these are used to mape out the first 4MB of memory in early boot
__attribute__((aligned(4096))) static volatile uint64_t g_kernel_pml4[512] = { 0 };

// these are used to map put the early identy mapping
__attribute__((aligned(4096))) static volatile uint64_t g_early_pdpt_lo[1] = { 0 };
__attribute__((aligned(4096))) static volatile uint64_t g_early_pd_lo[EARLY_MAP_NUM_PAGES_2MiB] = { 0 };

// these are use to map the upper half of virtual memory
__attribute__((aligned(4096))) static volatile uint64_t g_early_pdpt_hi[1] = { 0 };
__attribute__((aligned(4096))) static volatile uint64_t g_early_pd_hi[EARLY_MAP_NUM_PAGES_2MiB] = { 0 };

// these are used to do the direct mapping of physical memory
// into kernel space
__attribute__((aligned(4096))) static volatile uint64_t g_phys_to_virt_pdpt[512] = { 0 };

#define PAGE_MASK_4KiB (0xFFFFFFFFFFFFF000)

static uint64_t next_free_phys = 0x200000;
static uint64_t next_free_heap = MEM_START_KERNEL_HEAP;

// this  function sets up the identity mapping of the
// physical memory into the designated virtual kernel space
// region. this (among other things) needed to directly
// manipulate paging tables
void mem_setup_direct_map_phys_to_virt() {
    
    uint16_t pml4_index = mem_pml4_index(MEM_START_DIRECT_PHYS_TO_VIRT);
    g_kernel_pml4[pml4_index] = (uint64_t)g_phys_to_virt_pdpt | PAGE_PRESENT | PAGE_RW;
    for (int i = 0; i < DIRECT_MAP_NUM_PAGES_1GiB; i++) {
        g_phys_to_virt_pdpt[i] = (i * PAGE_SIZE_1GiB) | PAGE_PS | PAGE_PRESENT | PAGE_RW;
    }

}

// boot.S laid out the first 2MB of memory setting
// PS bit of the pde to 1. Let's build out more pde's
// of 2MB pages to map out more memory (possibly
// configurable during compile time)
void mem_setup_early_identity_mapping() {

    g_kernel_pml4[0] = (uint64_t)g_early_pdpt_lo | PAGE_PRESENT | PAGE_RW;
    g_kernel_pml4[256] = (uint64_t)g_early_pdpt_hi | PAGE_PRESENT | PAGE_RW;
    g_early_pdpt_lo[0] = (uint64_t)g_early_pd_lo | PAGE_PRESENT | PAGE_RW;
    g_early_pdpt_hi[0] = (uint64_t)g_early_pd_hi | PAGE_PRESENT | PAGE_RW;

    for (int i = 0; i < EARLY_MAP_NUM_PAGES_2MiB; i++) {
        g_early_pd_lo[i] = (i * PAGE_SIZE_2MiB) | PAGE_PS | PAGE_PRESENT | PAGE_RW;
        g_early_pd_hi[i] = (i * PAGE_SIZE_2MiB) | PAGE_PS | PAGE_PRESENT | PAGE_RW;
    }
    
    __asm__ volatile ("mov %0, %%cr3" :: "r"((g_kernel_pml4)));

}

// bump allocator for physical memory
// TODO: track the size of the allocated memory
// TODO: use better allocation strategy
void * mem_alloc_phys(uint64_t num) {
    void * ret_addr = (void *)next_free_phys;
    next_free_phys += num * PAGE_SIZE_4KiB;
    return ret_addr;
}

// bump allocator for virtual memory,
// it should guarantee that the memory is
// contiguous
// TODO: track the size of the allocated memory
// TODO: use better allocation strategy
void * mem_alloc_heap(uint64_t num) {
    void * ret_addr = (void *)next_free_heap;
    next_free_heap += num * PAGE_SIZE_4KiB;
    return ret_addr;
}

// convert a physical address into the correstponding
// directly mapped virtual address
void * mem_phys_to_virt(uint64_t phys) {
    return (void*)(phys + MEM_START_DIRECT_PHYS_TO_VIRT);
}

// map physical frame into virtual memory
void mem_map_page(uint64_t phys, uint64_t virt) {
    
    uint16_t pml4_index = mem_pml4_index(virt);
    uint16_t pdpt_index = mem_pdpt_index(virt);
    uint16_t pd_index = mem_pd_index(virt);
    uint16_t pt_index = mem_pt_index(virt);

    uint64_t * tmp_pdpt;
    uint64_t * tmp_pd;
    uint64_t * tmp_pt;

    if (!(g_kernel_pml4[pml4_index] & PAGE_PRESENT)) {
        void * new_pdpt = mem_phys_to_virt(mem_alloc_phys(1));
        memset(new_pdpt, 0, PAGE_SIZE_4KiB);
        g_kernel_pml4[pml4_index] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_RW;
    }

    tmp_pdpt = (uint64_t*)mem_phys_to_virt(g_kernel_pml4[pml4_index] & PAGE_MASK_4KiB);

    if (!(tmp_pdpt[pdpt_index] & PAGE_PRESENT)) {
        void * new_pd_phys = mem_alloc_phys(1);
        void * new_pd = mem_phys_to_virt(new_pd_phys);
        memset(new_pd, 0, PAGE_SIZE_4KiB);
        tmp_pdpt[pdpt_index] = (uint64_t)new_pd_phys | PAGE_PRESENT | PAGE_RW;
    }

    tmp_pd = (uint64_t*)mem_phys_to_virt(tmp_pdpt[pdpt_index] & PAGE_MASK_4KiB);
    
    if (!(tmp_pd[pd_index] & PAGE_PRESENT)) {
        void * new_pt_phys = mem_alloc_phys(1);
        void * new_pt = mem_phys_to_virt(new_pt_phys);
        memset(new_pt, 0, PAGE_SIZE_4KiB);
        tmp_pd[pd_index] = (uint64_t)new_pt_phys | PAGE_PRESENT | PAGE_RW;
    }

    tmp_pt = (uint64_t*)mem_phys_to_virt(tmp_pd[pd_index] & PAGE_MASK_4KiB);
    
    for (int i = 0; i < 512; i++) {
        // make sure we use 4KiB aligned physical frames
        tmp_pt[i] = (phys & PAGE_MASK_4KiB) + i * PAGE_SIZE_4KiB | PAGE_PRESENT | PAGE_RW;
    }

}

// allocate and map several pages and return the
// virtual base address of the first page
void * mem_alloc_pages(uint64_t num_pages) {
    
    void * virt = mem_alloc_heap(num_pages);

    for (int i = 0; i < num_pages; i++) {
        uint64_t page_frame_addr = mem_alloc_phys(1);
        mem_map_page(page_frame_addr, virt + i * PAGE_SIZE_4KiB);
    }

}

// allocate and map stack pages. the difference
// to mem_alloc_pages is that the stack grows downwards
// and the stack pointer is set to the top of the stack
void * mem_alloc_stack_pages(uint16_t num_pages) {
    
    void * stack_stop = (uint64_t)mem_alloc_pages(num_pages);
    void * stack_start = (uint64_t)stack_stop + num_pages * PAGE_SIZE_4KiB - 1;
    return stack_start;

}