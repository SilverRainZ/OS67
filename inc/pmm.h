#ifndef __PMM_H
#define __PMM_H

#include <type.h>
/* memory type */
#define STACK_SIZE 8192
extern uint32_t kern_stack_top;


#define ADDR_RANGE_MEMORY   1
#define ADDR_RANGE_RESERVED 2
#define ADDR_RANGE_UNDEFINE 3

#define PMM_MAX_SIZE 0x20000000 // 512M
#define PMM_PAGE_SIZE 0x1000
#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)
#define PMM_PAGW_MASK 0xfffff000

struct ARD_entry_s {
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
}__attribute__((packed));

void pmm_mem_info();
void pmm_init();
uint32_t pmm_alloc_page();
void pmm_free_page(uint32_t addr);
void pmm_test();

#endif
