#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC      0xe85250d6
#define MULTIBOOT2_HEADER_ARCH       0
#define MULTIBOOT2_HEADER_ALIGN      8
#define MULTIBOOT2_HEADER_TAG_END    0

#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_TAG_TYPE_MMAP 6

struct multiboot2_header_preamble {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
} __attribute__((packed));

struct multiboot2_header_tag {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((packed));

struct multiboot2_header_tag_memmap_req {
    uint16_t type;          // 2 bytes
    uint16_t flags;         // 2 bytes
    uint32_t size;          // 4 bytes
    uint32_t requests[1];   // 4 bytes
    uint32_t padding;       // 4 bytes
                            // = 16 bytes
} __attribute__((packed)); 

struct multiboot2_header_tag_end {
    uint16_t type;      // 2 bytes
    uint16_t flags;     // 2 bytes
    uint32_t size;      // 4 bytes
                        // = 8 bytes
} __attribute__((packed));

struct multiboot2_header {
    struct multiboot2_header_preamble preamble;
    struct multiboot2_header_tag_memmap_req memmap_tag;
    struct multiboot2_header_tag_end end_tag;
} __attribute__((packed));

#endif