/* vmm.h
 * This file is modified form hurlex 
 * 虚拟内存映射
 * 建立正式的页目录和页表, 映射了512M内存(尽管实际上可能没有这么大)，
 * 初始时物理地址和虚拟地址相等
 * 同时实现了对页异常 int 14 的处理 page_fault()
 * 实现了map和unmap，供kmalloc使用
 */
// std
#include <type.h>
#include <asm.h>
#include <dbg.h>
// x86
#include <pm.h>
#include <isr.h>
// libs
#include <string.h>
#include <printk.h>
// mm
#include <vmm.h> 
#include <pmm.h>
// proc
#include <proc.h>


extern uint8_t kernstart;
extern uint8_t kernend;

extern uint8_t __init_start;
extern uint8_t __init_end;

pde_t pgd_kern[PDE_SIZE] __attribute__((aligned(PAGE_SIZE)));
/* page table entry of kernel */
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__((aligned(PAGE_SIZE)));

/* switch page table */
inline void vmm_switch_pgd(uint32_t pde){
    __asm__ volatile ("mov %0, %%cr3": :"r"(pde));
}

/* reflush page table cache */
static inline void vmm_reflush(uint32_t va){
    __asm__ volatile ("invlpg (%0)"::"a"(va));
}

static inline void vmm_enable(){
    uint32_t cr0;

    __asm__ volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= CRO_PG;
	__asm__ volatile ("mov %0, %%cr0" : : "r" (cr0));
}

void vmm_init(){
    uint32_t i, j;
    /* register isr */
    idt_install(FAULT_PAGE, (unsigned)page_fault, SEL_KERN_CODE, GATE_INT, IDT_PR|IDT_DPL_KERN);

    for (i = 0, j = 0; i < PTE_COUNT; i++, j++){
        pgd_kern[i] = (uint32_t)pte_kern[j] | PTE_P | PTE_R | PTE_K;
    }
    
    uint32_t *pte = (uint32_t *)pte_kern;
    for (i = 1; i < PTE_COUNT*PTE_SIZE; i++){
        pte[i] = (i << 12) | PTE_P | PTE_P | PTE_K;
    }

    vmm_switch_pgd((uint32_t)pgd_kern);

    vmm_enable();
}

void vmm_map(pde_t *pgdir, uint32_t va, uint32_t pa, uint32_t flags){
    uint32_t pde_idx = PDE_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // printl("map: map 0x%x to 0x%x, flag = 0x%x\n", pa, va, flags);
 
    pte_t *pte = (pte_t *)(pgdir[pde_idx] & PAGE_MASK);
    /* if pte == NULL */
    if (!pte){
        // printl("map: pte of 0x%x is NULL, attempt to alloc one\n", va);
        pte = (pte_t *)pmm_alloc();
        pgdir[pde_idx] = (uint32_t)pte | PTE_P | PTE_R | flags;
        //assert(0,"pet = NULL");
        memset(pte, 0, PAGE_SIZE);
    } 

    pte[pte_idx] = (pa & PAGE_MASK) | flags;

    vmm_reflush(va);
    // printl("map: ret\n");
}

void vmm_unmap(pde_t *pde, uint32_t va){
    uint32_t pde_idx = PDE_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // printl("unmap: unmap virtual address 0x%x\n", va);
    pte_t *pte = (pte_t *)(pde[pde_idx] & PAGE_MASK);

    //assert(pte,"unmap fail.");
    if (!pte){
        // printl("unmap: unmap a unmapped page\n");
        return;
    }
    /* unmap this poge */
    pte[pte_idx] = 0;

    vmm_reflush(va);
}

int vmm_get_mapping(pde_t *pgdir, uint32_t va, uint32_t *pa){
    uint32_t pde_idx = PDE_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t *)(pgdir[pde_idx] & PAGE_MASK);
    if (!pte){
        printl("get_mapping: virtual address 0x%x is unmapped\n", va);
        return 0;
    }
    if (pte[pte_idx] != 0 && pa){
        *pa = pte[pte_idx] & PAGE_MASK;
        printl("get_mapping: virtual address 0x%x is mapped to 0x%x\n", va, pte[pte_idx] & PAGE_MASK);
        return 1;
    }
    return 0;
}

void vmm_test(){
    /* 注意映射的的粒度是一页，把 map(va = 0xc000, pa = 0x1234), 
     * 你得到的是（0x1000,0x1fff）- (0xc000,0xcfff) 的映射
     * 完成映射后，物理地址 0x1234 的虚拟地址是 0xc234 */
    printl("=== vmm_test start ===\n");
    printl("=== vmm_test end ===\n");
}

void page_fault(struct int_frame *r){
    uint32_t cr2;
    __asm__ volatile ("mov %%cr2, %0":"=r"(cr2));
    panic("Page Fault Excetpion\nSystem Halted!\n");
    for (;;) hlt();
}

void kvm_init(pde_t *pgdir){
    uint32_t addr;
    uint32_t limit = PAGE_ALIGN_UP((uint32_t)&kernend);

    assert(pgdir != 0, "kvm_init: null pgdir");

    for (addr = 0; addr < limit; addr += PAGE_SIZE){
        vmm_map(pgdir, addr, addr, PTE_P | PTE_R | PTE_K);
    }
}

void uvm_init_fst(pde_t *pgdir, char *init, uint32_t size){
    char *room;

    assert(size < PAGE_SIZE, "uvm_init_fst: size");

    printl("uvm_init_fst: alloc memory fot init\n");
    room = (char *)pmm_alloc();

    memset(room, 0, PAGE_SIZE);
    printl("init = 0x%x size = %d\n", init, size);
    memcpy(room, init, size);
    printl("uvm_init_fst: clear and copy\n");

    vmm_map(pgdir, USER_BASE, (uint32_t)room, PTE_U | PTE_P | PTE_R);
    printl("uvm_init_fst: map\n");
}

void uvm_switch(struct proc *pp){
    cli();

    tss_set(SEL_KERN_DATA, (uint32_t)pp->kern_stack + PAGE_SIZE);
    vmm_switch_pgd((uint32_t)pp->pgdir);

    sti();
}
