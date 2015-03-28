#include <type.h>
#include <asm.h>
#include <pm.h>
#include <isr.h>
#include <vga.h>
#include <timer.h>
#include <kb.h>
#include <string.h>
#include <printk.h>
#include <pmm.h>
void delay(){   // lol, we have no sleep()
    int i = 0;
    for (i = 0; i < 1000000; i++){
        i++;
        i--;
    }
}

int osmain(void)
{
    // 847d
    vga_init();
    gdt_init();
    puts("GDT installed...\n\r");
    idt_init();
    puts("IDT installed...\n\r");
    isrs_init();
    puts("ISRs installed...\n\r");
    irq_init();
    puts("IRQs installed...\n\r");

    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);
    //timer_init(); 
    kb_init();
    sti();
    pmm_mem_info();
    pmm_init();
    uint32_t p = pmm_alloc_page();
    printk("alloc 4k page: 0x%x\n",p);
    p = pmm_alloc_page();
    printk("alloc 4k page: 0x%x\n",p);
    pmm_free_page(p);
    printk("free 4k page: 0x%x\n",p);
    p = pmm_alloc_page();
    printk("alloc 4k page: 0x%x\n",p);
    p = pmm_alloc_page();
    printk("alloc 4k page: 0x%x\n",p);
    p = pmm_alloc_page();
    printk("alloc 4k page: 0x%x\n",p);
    for (;;);
    int i;
    for (i = 0; i < 26; i++){
        putchar('A' + i);
        putchar('\n');
        delay();
    }
    return 0;
}
