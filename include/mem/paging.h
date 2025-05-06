// paging.h
#pragma once

#include <stdint.h>

#define PAGE_PRESENT ( 0x01 )
#define PAGE_RW      ( 0x02 )
#define PAGE_USER    ( 0x04 )
#define PAGE_PS      ( 0x80 )

#define PAGE_SIZE_4KiB    ( 0x1000ULL )
#define PAGE_SIZE_2MiB    ( 0x200000ULL )
#define PAGE_SIZE_1GiB    ( 0x40000000ULL )

#define EARLY_MAP_NUM_PAGES_2MiB  ( 2ULL )
#define DIRECT_MAP_NUM_PAGES_1GiB ( 4ULL )

#define NUM_PAGES_FOR_TYPE(type) (((sizeof(type) + PAGE_SIZE_4KiB - 1) / PAGE_SIZE_4KiB))

void mem_setup_direct_map_phys_to_virt();
void mem_setup_early_identity_mapping();
void * mem_alloc_phys(uint64_t num);
void * mem_alloc_heap(uint64_t num);
void * mem_phys_to_virt(uint64_t phys);
void mem_map_page(uint64_t phys, uint64_t virt);
void * mem_alloc_pages(uint64_t num_pages);
void * mem_alloc_stack_pages(uint16_t num_pages);


inline uint16_t mem_pml4_index(uint64_t addr) { return (addr >> 39) & 0x1FF; }
inline uint16_t mem_pdpt_index(uint64_t addr) { return (addr >> 30) & 0x1FF; }
inline uint16_t mem_pd_index(uint64_t addr)   { return (addr >> 21) & 0x1FF; }
inline uint16_t mem_pt_index(uint64_t addr)   { return (addr >> 12) & 0x1FF; }

void* memcpy(void* dest, const void* src, uint64_t n);

