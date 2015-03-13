#include <type.h> 
#include <printk.h>
#include <pmm.h>

/* these symbol's addr were remaped in ld script*/
uint8_t kernstart = 0; 
uint8_t code = 0;
uint8_t data = 0;
uint8_t bss = 0;
uint8_t kernend = 0; 

void pmm_mem_info(){
    /*get memory size */
    uint16_t mem = *(uint16_t *)0x502;
    printk("Memory: %dMB.\n\r",mem/1024 + 1);

    /*get kernel mem map*/
    printk("kernel start at: 0x%x\n", &kernstart);
    printk("code section located at: 0x%x\n", &code);
    printk("data section located at: 0x%x\n", &data);
    printk("bss  section located at: 0x%x\n", &bss);
    printk("kernel end at: 0x%x\n", &kernend);
    printk("kernel's len : 0x%xkb\n", (&kernend - &kernstart)/1024);
    return;
}
