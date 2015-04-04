#include <type.h>
#include <vmm.h>
#include <pmm.h>
#include <pm.h>
#include <isr.h>
#include <string.h>
#include <printk.h>
#include <asm.h>

/* page directory table of kernel */
pgd_t pgd_kern[PGD_SIZE] __attribute__((aligned(PAGE_SIZE)));

/* page table entry of kernel */
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void vmm_init(){
    uint32_t i, j;
    for (i = 0, j = 0; i < PTE_COUNT; i++, j++){
        pgd_kern[i] = (uint32_t)pte_kern[j] | PAGE_PRESENT | PAGE_WRITE;
    }
    //TODO 为什么不映射第0页?
    uint32_t *pte = (uint32_t *)pte_kern;
    for (i = 1; i < PTE_COUNT*PTE_SIZE; i++){
        pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }
    printk("0x%x -- 0x%x\n", pte_kern,&pte_kern[PTE_COUNT][PTE_SIZE]);
    /* register isr */
    idt_set_gate(14, (unsigned)page_fault, 0x08, 0x8E);
    /* switch page global directory */
    vmm_switch_pgd((uint32_t)pgd_kern);
}

void vmm_switch_pgd(uint32_t pgd){
    __asm__ volatile ("mov %0, %%cr3"::"r"(pgd));
}
void map(pgd_t *pgd, uint32_t va, uint32_t pa, uint32_t flags){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);
    /* if pte == NULL */
    if (!pte){
        pte = (pte_t *)pmm_alloc_page();
        pgd[pgd_idx] = (uint32_t)pte;

        memset(pte, 0, PAGE_SIZE);
    } 
    //TODO ? & PAGE_MASK 
    pte[pte_idx] = (pa & PAGE_MASK) | flags;
}

void unmap(pgd_t *pgd, uint32_t va){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);
    if (!pte){
        return;
    }
    /* unmap this poge */
    pte[pte_idx] = 0;
    /* flush page table cache */
    __asm__ volatile ("invlpg (%0)"::"a"(va));
}

bool get_mapping(pgd_t *pgd, uint32_t va, uint32_t *pa){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);
    if (!pte){
        return FALSE;
    }
    if (pte[pte_idx] != 0 && pa){
        *pa = pte[pte_idx] & PAGE_MASK;
        return TRUE;
    }
    return FALSE;
}

void page_fault(struct regs_s *regs){
    uint32_t cr2;
    __asm__ volatile ("mov %%cr2, %0":"=r"(cr2));
    printk("page fault...");
    hlt();
}
