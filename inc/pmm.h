#ifndef __PMM_H
#include <type.h>
#define ADDR_RANGE_MEMORY   1
#define ADDR_RANGE_RESERVED 2
#define ADDR_RANGE_UNDEFINE 3

struct ARD_entry {
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
}__attribute__((packed));

/* these symbol's addr were remaped in ld script*/
uint8_t kernstart = 0; 
uint8_t code = 0;
uint8_t data = 0;
uint8_t bss = 0;
uint8_t kernend = 0; 

void pmm_mem_info();
#endif
