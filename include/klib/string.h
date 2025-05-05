#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <stdint.h>

void *memset(void* ptr, int value, uint64_t num);
void *memcpy(void * dest, const void * src, uint64_t n);
uint64_t strlen(const char *str);

#endif 