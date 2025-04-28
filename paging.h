// paging.h
#pragma once

#include <stdint.h>

#define PAGE_PRESENT (1ull << 0)
#define PAGE_RW      (1ull << 1)
#define PAGE_USER    (1ull << 2)
#define PAGE_SIZE 0x1000 // 4 KiB

#define NUM_PAGES_FOR_TYPE(type) (((sizeof(type) + PAGE_SIZE - 1) / PAGE_SIZE))

void * early_alloc_single_page();
void * early_alloc_continuous_pages(uint16_t num_pages);
void * early_alloc_stack(uint16_t num_pages);
void setup_identity_paging();

static inline uint16_t pml4_index(uint64_t addr) { return (addr >> 39) & 0x1FF; }
static inline uint16_t pdpt_index(uint64_t addr) { return (addr >> 30) & 0x1FF; }
static inline uint16_t pd_index(uint64_t addr)   { return (addr >> 21) & 0x1FF; }
static inline uint16_t pt_index(uint64_t addr)   { return (addr >> 12) & 0x1FF; }

void* memcpy(void* dest, const void* src, uint64_t n);