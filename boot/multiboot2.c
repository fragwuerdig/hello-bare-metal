#include <boot/multiboot2.h>

__attribute__((section(".multiboot2"), aligned(MULTIBOOT2_HEADER_ALIGN)))
const struct multiboot2_header MB2H = {
    .preamble = {
        .magic = MULTIBOOT2_HEADER_MAGIC,
        .architecture = MULTIBOOT2_HEADER_ARCH,
        .header_length = sizeof(struct multiboot2_header),
        .checksum = -(MULTIBOOT2_HEADER_MAGIC +
                      MULTIBOOT2_HEADER_ARCH +
                      sizeof(struct multiboot2_header)),
    },
    .memmap_tag = {
        .type = MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST,
        .flags = 0,
        .size = sizeof(struct multiboot2_header_tag_memmap_req),
        .requests = { MULTIBOOT_TAG_TYPE_MMAP },
        .padding = 0,
    },
    .end_tag = {
        .type = MULTIBOOT2_HEADER_TAG_END,
        .flags = 0,
        .size = 8,
    }
};