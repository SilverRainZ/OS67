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
#include <vmm.h>
#include <dbg.h>
#include <heap.h>

void delay(){   // lol, we have no sleep()
    int i = 0;
    for (i = 0; i < 1000000; i++){
        i++;
        i--;
    }
}

void foo(){
    assert(0,"test");
}
int osmain(void)
{
    vga_init();
    gdt_init();
    puts("GDT installed...\n\r");
    idt_init();
    puts("IDT installed...\n\r");
    isrs_init();
    puts("ISRs installed...\n\r");
    irq_init();
    puts("IRQs installed...\n\r");
    pmm_init();
    //timer_init(); 
    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);
    sti();
    pmm_mem_info();
    vmm_init();
    kb_init();
    //vmm_test();
    heap_init();
    heap_test();

    foo();
    for (;;);
    return 0;
}
