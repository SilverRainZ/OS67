/* pmm.h
 * This file is modified form hurlex 
 * 内存物理页框分配，用栈管理从0x100000开始的所有有效物理地址
 * 同时提供了利用 int 0xe820 获取内存布局的函数
 * （该中断的执行在 boot/bootsector.asm:get_mem_map 中）
 * 内核的布局通过在ldscript中定位的变量获取
 */

#include <type.h> 
#include <printk.h>
#include <pmm.h>

/* these symbol's addr were remaped in ld script*/
uint8_t kernstart = 1; 
uint8_t code = 2;
uint8_t data = 3;
uint8_t bss = 4;
uint8_t kernend = 5; 


static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];
static uint32_t pmm_stack_top = 0;     // top of stack
uint32_t pmm_page_count = 0;

void pmm_mem_info(){
    /* 0x400 和 0x500 中的数据在bootsector.asm 中已经写入 */
    uint32_t MCR_count = *(uint32_t *)0x400;
    struct ARD_entry_s *ARD_entry = (struct ARD_entry_s *)0x500;
    uint32_t memsize = 0;
    int i = 0;
    /* do not care about high bit, we assume that the size of
     * memory always less than 512M */
    for (i = 0; i < MCR_count; i++){
        printk("base addr low: 0x%x  ",ARD_entry[i].base_addr_low);
        printk("len low: 0x%x  ",ARD_entry[i].len_low);
        printk("type: 0x%x\n",ARD_entry[i].type);
        if (ARD_entry[i].type == ADDR_RANGE_MEMORY
                && ARD_entry[i].base_addr_low + ARD_entry[i].len_low > memsize){
            memsize = ARD_entry[i].base_addr_low + ARD_entry[i].len_low;
        }
    }
    printk("memory size: %dMB\n",memsize/(1024*1024) + 1);
    /*get kernel mem map*/
    printk("kernel start at: 0x%x  end at 0x%x\n", &kernstart, &kernend);
    printk(".code: 0x%x  ", &code);
    printk(".data: 0x%x  ", &data);
    printk(".bss: 0x%x  \n", &bss);
    printk("kernel's size: %dkb\n", (&kernend - &kernstart)/1024);
}

void pmm_init(){
    uint32_t MCR_count = *(uint32_t *)0x400;
    struct ARD_entry_s *ARD_entry = (struct ARD_entry_s *)0x500;
    int i = 0;
    for (i = 0; i < MCR_count; i++){
        /* reach available memory */
        /* 如果该内存段有效且基址是0x100000, 将其加入栈中 */
        /* TODO 我觉得这里应该从 kernend 之后再开始分配 */
       if (ARD_entry[i].type == ADDR_RANGE_MEMORY && ARD_entry[i].base_addr_low == 0x100000) {
           uint32_t addr = ARD_entry[i].base_addr_low;
           uint32_t limit = addr + ARD_entry[i].len_low;
           while (addr < limit && addr <= PMM_MAX_SIZE){
               pmm_free_page(addr);
               addr += PMM_PAGE_SIZE;
               pmm_page_count++;
           }
       }
    }
}

uint32_t pmm_alloc_page(){
    //TODO assert()
    return pmm_stack[--pmm_stack_top];
}
void pmm_free_page(uint32_t addr){
    //TODO assert()
    pmm_stack[pmm_stack_top++] = addr;
}
