// std
#include <type.h>
#include <dbg.h>
// libs
#include <string.h>
#include <printk.h>
// mm
#include <pmm.h>
#include <vmm.h>
// proc
#include <proc.h>

extern uint8_t kernstart;
extern uint8_t kernend;

extern uint8_t __init_start;
extern uint8_t __init_end;

void kvm_init(pde_t *pgdir){
    uint32_t i;
    uint32_t limit = PAGE_ALIGN_UP((uint32_t)&kernend);

    assert(pgdir != 0, "kvm_init: null pgdir");

    for (i = 0; i < limit; i += PAGE_SIZE){
        vmm_map(pgdir, i, i, PTE_P | PTE_R | PTE_K);
    }
}

void uvm_init_fst(pde_t *pgdir, char *init, uint32_t size){
    char *room;

    assert(size < PAGE_SIZE, "uvm_init_fst: size");

    room = (char *)pmm_alloc();

    memset(room, 0, PAGE_SIZE);
    memcpy(room, init, size);

    vmm_map(pgdir, USER_BASE, (uint32_t)room, PTE_U | PTE_P | PTE_R);
}

void uvm_init(){

}

void uvm_switch(){

}
