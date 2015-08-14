// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
// mm
#include <pmm.h>
#include <vmm.h>
// proc
#include <proc.h>

extern uint8_t kernstart;
extern uint8_t kernend;

void kvm_init(pde_t *pgdir){
    uint32_t i;
    uint32_t limit = PAGE_ALIGN_UP((uint32_t)&kernend);

    assert(pgdir != 0, "kvm_init: null pgdir");

    for (i = 0; i < limit; i += PAGE_SIZE){
        vmm_map(pgdir, i, i, PTE_P | PTE_R | PTE_K);
    }
}

void uvm_init(){

}

void uvm_switch(){

}
