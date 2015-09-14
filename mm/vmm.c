#define __LOG_ON 1
/* vmm.h
 * 
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
// fs
#include <minix.h>
#include <inode.h>
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
    idt_install(FAULT_PAGE, (unsigned)page_fault, SEL_KCODE << 3, GATE_INT, IDT_PR|IDT_DPL_KERN);

    // map 4G memory, physcial address = virtual address
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

    if (!pte){
        if (va >= USER_BASE){
            pte = (pte_t *)pmm_alloc();
            memset(pte, 0, PAGE_SIZE);
            pgdir[pde_idx] = (uint32_t)pte | PTE_P | flags;
        } else {
            pte = (pte_t *)(pgd_kern[pde_idx] & PAGE_MASK);
            pgdir[pde_idx] = (uint32_t)pte | PTE_P | flags;
        }
    }

    pte[pte_idx] = (pa & PAGE_MASK) | PTE_P | flags;

    vmm_reflush(va);
}

void vmm_unmap(pde_t *pde, uint32_t va){
    uint32_t pde_idx = PDE_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    // printl("unmap: unmap virtual address 0x%x\n", va);
    pte_t *pte = (pte_t *)(pde[pde_idx] & PAGE_MASK);

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
    if (pte[pte_idx] != 0 && (pte[pte_idx] & PTE_P) && pa){
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

    printk("errro code1: 0x%x\n", cr2);
    printk("errro code2: 0x%x\n", r->err_code);

    panic("Page Fault Excetpion\nSystem Halted!\n");

    for (;;) hlt();
}

// build a map of kernel space for a process's page table
void kvm_init(pde_t *pgdir){
    uint32_t addr;
    // uint32_t limit = PAGE_ALIGN_UP((uint32_t)&kernend);

    assert(pgdir != 0, "kvm_init: null pgdir");

    for (addr = 0; addr < USER_BASE; addr += PAGE_SIZE){
        vmm_map(pgdir, addr, addr, PTE_P | PTE_R | PTE_K);
    }
}

/* build a map of user space for a initproc's page table */
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
    tss_set(SEL_KDATA << 3, (uint32_t)pp->kern_stack + PAGE_SIZE);
    vmm_switch_pgd((uint32_t)pp->pgdir);
}

/* only work when pagetable = gpd_kern */
int uvm_load(pte_t *pgdir, uint32_t addr, struct inode *ip, uint32_t off, uint32_t size){
    uint32_t i, n, pa;

    printl("uvm_load: pgdir: 0x%x addr: 0x%x ip: inode-%d offset: 0x%x size: 0x%x\n", pgdir, addr, ip->ino, off, size);

    assert((uint32_t)addr % PAGE_SIZE == 0, "uvm_load: addr must page aligned");

    for (i = 0; i < size; i += PAGE_SIZE){
        assert(vmm_get_mapping(pgdir, addr + i, &pa), "uvm_load: address no mapped");

        if (size - i < PAGE_SIZE){
            n = size - 1;
        } else {
            n = PAGE_SIZE;
        }

        // pa = va now
        if (iread(ip, (char *)pa, off + i, n) != (int)n){
            return -1;
        }
    }
    return 0;
}

/* free user space only */
void uvm_free(pte_t *pgdir){
    uint32_t i;

    assert(pgdir, "uvm_free: no page table");

    i = PDE_INDEX(USER_BASE);

    printl("uvm_free: form 0x%x to 0x%x\n", i << 22, PTE_COUNT << 22);

    for (; i < PTE_COUNT; i++){
        if (pgdir[i] & PTE_P){
            printl("uvm_free: free pte 0x%x\n", pgdir[i]);
            pmm_free(pgdir[i] & PAGE_MASK);
        }
    }

    pmm_free((uint32_t)pgdir);
}

pde_t *uvm_copy(pte_t *pgdir, uint32_t size){
    pde_t *pgd;
    uint32_t i, pa, mem;

    pgd = (pde_t *)pmm_alloc();
    kvm_init(pgd);

    printl("uvm_copy: copy pgdir 0x%x -> 0x%x, size: %d\n",pgdir, pgd, size);

    for (i = 0; i < size; i += PAGE_SIZE){
        assert(vmm_get_mapping(pgdir, USER_BASE + i, &pa),"uvm_copy: pte not exixt or no present");

        mem = pmm_alloc();

        printl("uvm_copy: phyaddr: 0x%x -> 0x%x\n", pa, mem);

        memcpy((void *)mem, (void *)pa, PAGE_SIZE);
        vmm_map(pgd, USER_BASE + i, mem, PTE_R | PTE_U | PTE_P); // TODO (?)
    }
    return pgd;
}

int uvm_alloc(pte_t *pgdir, uint32_t old_sz, uint32_t new_sz){
    uint32_t mem;
    uint32_t start;

    printl("uvm_alloc: pgdir: 0x%x 0x%x -> 0x%x\n", pgdir, old_sz, new_sz);

    if (new_sz < old_sz){
        return old_sz;
    }

    for (start = PAGE_ALIGN_UP(old_sz); start < new_sz; start += PAGE_SIZE){
        mem = pmm_alloc(); 
        memset((void *)mem, 0, PAGE_SIZE);
        vmm_map(pgdir, start, mem, PTE_P | PTE_R | PTE_U);  // diff with PTE_U PTE_R ?
    }

    return new_sz;
}
