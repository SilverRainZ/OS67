#include <type.h> 
#include <printk.h>
#include <pmm.h>



//static uint32_t pmm_alloc_stack[];
void pmm_mem_info(){
    uint32_t MCR_count = *(uint32_t *)0x400;
    struct ARD_entry *ARD_arr = (struct ARD_entry *)0x500;

    uint32_t memsize = 0;

    int i = 0;
    /* do not care about high bit, we assume that the size of
     * memory always less than 512M */
    for (i = 0; i < MCR_count; i++){
        printk("base addr low: 0x%x  ",ARD_arr[i].base_addr_low);
        printk("len low: 0x%x  ",ARD_arr[i].len_low);
        printk("type: 0x%x\n",ARD_arr[i].type);
        if (ARD_arr[i].type == ADDR_RANGE_MEMORY
                && ARD_arr[i].base_addr_low + ARD_arr[i].len_low > memsize){
            memsize = ARD_arr[i].base_addr_low + ARD_arr[i].len_low;
        }
    }
    printk("memory size: %dMB\n",memsize/(1024*1024) + 1);
    /*get kernel mem map*/
    printk("kernel start at: 0x%x  ", &kernstart);
    printk(".code: 0x%x  ", &code);
    printk(".text: 0x%x  ", &data);
    printk(".bss: 0x%x\n", &bss);
    printk("kernel's size: %dkb\n", (&kernend - &kernstart)/1024);
    return;
}

void pmm_init(){

}
