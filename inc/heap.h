#ifndef __HEAP_H
#define __HEAP_H
#include <type.h>

#define HEAP_START 0xc0000000

struct header{
    struct header *prev;
    struct header *next;
    uint32_t allocated: 1; 
    uint32_t length : 31;
};

void heap_init();
void *kmalloc(uint32_t len);
void kfree(void* p);
void heap_test();

#endif
