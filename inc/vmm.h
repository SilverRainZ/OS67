#ifndef __VMM_H
#define __VMM_H

#include <type.h>
#include <isr.h>

/* pgd&pdt attribute */
#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

/* 4k per page */
#define PAGE_SIZE 4096 

#define PAGE_MASK 0xfffff000

/* get specific index of virtual address */
#define PGD_INDEX(x) (((x) >> 22) & 0x3ff)
#define PTE_INDEX(x) (((x) >> 12) & 0x3ff)
#define OFFSET_INDEX(x) ((x) & 0xfff)

/* page global directory */
typedef uint32_t pgd_t;
/* page talbe entry */
typedef uint32_t pte_t;

/* size of page global dirctory */
#define PGD_SIZE (PAGE_SIZE/sizeof(pte_t))
/* size of page talbe entry */
#define PTE_SIZE (PAGE_SIZE/sizeof(uint32_t))

/* 512M */
#define PTE_COUNT 128

/* export page dirctroy */
extern pgd_t pgd_kern[PGD_SIZE];

void vmm_init();
void vmm_switch_pgd(uint32_t pgd);
void map(pgd_t *pgd, uint32_t va, uint32_t pa, uint32_t flags);
void unmap(pgd_t *pgd, uint32_t va);
bool get_mapping(pgd_t *pgd, uint32_t va, uint32_t *pa);
void vmm_test();

/* isr 14 */
void page_fault(struct regs_s *regs);
#endif
