#include <stdint.h>
#include "paging.h"
#include "klib/string.h"

// early page table layout
__attribute__((aligned(4096))) static volatile uint64_t pml4[1];
__attribute__((aligned(4096))) static volatile uint64_t pdpt[1];
__attribute__((aligned(4096))) static volatile uint64_t pd[512];
__attribute__((aligned(4096))) static volatile uint64_t pt[512];

#define PML4_BASE (uint64_t)(pml4)
#define PDPT_BASE (uint64_t)(pdpt)
#define PD_BASE  (uint64_t)(pd)
#define PT_BASE  (uint64_t)(pt)


static uint64_t next_free_phys = 0x001ff000;

// Identity mapping for the first 2MB of memory
void setup_identity_paging() {
    
    pml4[0] = 0;
    pdpt[0] = 0;
    pd[0] = 0;
    pd[0] = 0;

    for (int j = 0; j < 512; j++) {
        pt[j] = 0;
    }

    // Link hierarchy
    pml4[0] = PDPT_BASE | PAGE_PRESENT | PAGE_RW;
    pdpt[0] = PD_BASE | PAGE_PRESENT | PAGE_RW;
    pd[0] = PT_BASE + 0 * 8 * 512 | PAGE_PRESENT | PAGE_RW;

    for (int i = 0; i < 512; i++) {
        pt[i] = i * 0x1000 | PAGE_PRESENT | PAGE_RW;
    }

    // Load new page tables
    __asm__ volatile ("mov %0, %%cr3" :: "r"((PML4_BASE)));

}

void* early_alloc_continuous_pages(uint16_t num_pages) {
    
    if (num_pages == 0) {
        return 0;
    }

    if (num_pages + 2 > 32768) {
        return 0;
    }

    // +2 Seiten als Sicherheitspuffer
    uint16_t pages_to_prepare = num_pages + 2;          // = 3
    uint64_t future_phys = next_free_phys + PAGE_SIZE;  // == 0x200000

    for (uint16_t i = 0; i < pages_to_prepare; i++) {
        uint16_t pd_index_future = pd_index(future_phys); // == 1

        if (!(pd[pd_index_future] & PAGE_PRESENT)) {
            volatile uint64_t * new_pt_table = next_free_phys; // == 0x1ff000
            next_free_phys += PAGE_SIZE; // == 0x200000

            for (int j = 0; j < 512; j++) {
                // j = 0 > 0x200000
                // j = 1 > 0x200000 + 0x1000
                new_pt_table[j] = future_phys + j * 0x1000 | PAGE_PRESENT | PAGE_RW;
                /*if (i == 1023 && j == 511) {
                    return new_pt_table[j];
                    //return pt[0];
                }*/
            }
            pd[pd_index_future] = (uint64_t)new_pt_table | PAGE_PRESENT | PAGE_RW;
            /*if (i == 1024) {
                return pd[pd_index_future];
            }*/
        }
        future_phys += PAGE_SIZE;
    }

    void* start_addr = (void*)next_free_phys;
    next_free_phys += num_pages * PAGE_SIZE;

    return start_addr;
}

void* early_alloc_single_page() {
    
    return early_alloc_continuous_pages(1);

}

// the same as early_alloc_continuous_pages,
// but the stack grows downwards so we return
// the last address of the allocated memory
void * early_alloc_stack(uint16_t num_pages) {
    
    void * stack_stop = (uint64_t)early_alloc_continuous_pages(num_pages);
    void * stack_start = (uint64_t)stack_stop + num_pages * PAGE_SIZE - 1;
    return stack_start;

}