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
