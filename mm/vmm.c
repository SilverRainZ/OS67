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

/* page directory table of kernel */
pgd_t pgd_kern[PGD_SIZE] __attribute__((aligned(PAGE_SIZE)));
/* page table entry of kernel */
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__((aligned(PAGE_SIZE)));
/* 由于页目录和页表都aligned(PAGE_SIZE), 
 * 因此低地址的12位始终为0 */

void vmm_init(){
    uint32_t i, j;
    /*phyical addr = virtual addr */
    /* 为页目录填充128个页表的地址(高20位), 地址是物理地址，
     * 当然这里物理地址和虚拟地址相同 */
    for (i = 0, j = 0; i < PTE_COUNT; i++, j++){
        pgd_kern[i] = (uint32_t)pte_kern[j] | PAGE_PRESENT | PAGE_WRITE;
    }

    /* 初始化剩下的页目录 */
    for (; i < PGD_SIZE; i++){
        pgd_kern[i] = 0;
    } 

    /* 为所有的页表项填充映射到的页地址(高20位) 
     * hurlex 在这里没有映射第0页， 然而我并不知道为什么要这么做 */
    uint32_t *pte = (uint32_t *)pte_kern;
    for (i = 1; i < PTE_COUNT*PTE_SIZE; i++){
        pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }

    /* register isr */
    idt_set_gate(14, (unsigned)page_fault, 0x08, 0x8E);

    /* switch page global directory */
    vmm_switch_pgd((uint32_t)pgd_kern);

    /* enable paging */
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	__asm__ volatile ("mov %0, %%cr0" : : "r" (cr0));
}

void vmm_switch_pgd(uint32_t pgd){
    __asm__ volatile ("mov %0, %%cr3": :"r"(pgd));
}

void map(pgd_t *pgd, uint32_t va, uint32_t pa, uint32_t flags){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    printl("map: map 0x%x to 0x%x, flag = 0x%x\n", pa, va, flags);
 
    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);
    /* if pte == NULL */
    if (!pte){
    /* 只映射了0-512M的内存， 映射更高位的内存时，需要重新申请物理页
     * 我觉得这样增加了复杂度， 一开始就定义一个最大的1024*1024页表表不就好？*/
        printl("map: pte of 0x%x is NULL, attempt to alloc one\n", va);
        pte = (pte_t *)pmm_alloc_page();
        pgd[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;
        //assert(0,"pet = NULL");
        memset(pte, 0, PAGE_SIZE);
    } 

    pte[pte_idx] = (pa & PAGE_MASK) | flags;

    /* reflush page talbe cache  */
    __asm__ volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd, uint32_t va){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    printl("unmap: unmap virtual address 0x%x\n", va);
    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);

    //assert(pte,"unmap fail.");
    if (!pte){
        printl("unmap: unmap a unmapped page\n");
        return;
    }
    /* unmap this poge */
    pte[pte_idx] = 0;

    /* reflush page table cache */
    __asm__ volatile ("invlpg (%0)"::"a"(va));
}

bool get_mapping(pgd_t *pgd, uint32_t va, uint32_t *pa){
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);

    pte_t *pte = (pte_t *)(pgd[pgd_idx] & PAGE_MASK);
    if (!pte){
        printl("get_mapping: virtual address 0x%x is unmapped\n", va);
        return FALSE;
    }
    if (pte[pte_idx] != 0 && pa){
        *pa = pte[pte_idx] & PAGE_MASK;
        printl("get_mapping: virtual address 0x%x is mapped to 0x%x\n", va, pte[pte_idx] & PAGE_MASK);
        return TRUE;
    }
    return FALSE;
}

void vmm_test(){
    /* 注意映射的的粒度是一页，把 map(va = 0xc000, pa = 0x1234), 
     * 你得到的是（0x1000,0x1fff）- (0xc000,0xcfff) 的映射
     * 完成映射后，物理地址 0x1234 的虚拟地址是 0xc234 */
    printl("=== vmm_test start ===\n");
    int *badfood = (int *)0xc000;

    printl("valut at 0xc000: %x\n", *badfood);
    map(pgd_kern, 0x1000, 0xc000, PAGE_PRESENT);
    badfood = (int *)0x1000;

    uint32_t pa = 0;
    get_mapping(pgd_kern, 0x1000, &pa);

    vmm_switch_pgd((uint32_t)pgd_kern);

    printl("%x is mapped to: %x\n", (int)badfood, (int)pa);
    printl("value at 0x1000: %x\n", *badfood);

    //badfood = (int *)0xc0000000; 
    //printk("virtual addr 0xc000: %x\n", *badfood);    // page fault test
    printl("=== vmm_test end ===\n");
}

void page_fault(struct regs_s *regs){
    uint32_t cr2;
    __asm__ volatile ("mov %%cr2, %0":"=r"(cr2));
    panic("Page Fault Excetpion\nSystem Halted!\n");
    for (;;) hlt();
}
