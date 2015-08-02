// std
#include <type.h>
#include <asm.h>
#include <dbg.h>
// x86
#include <pm.h>
#include <isr.h>
#include <timer.h>
// libs
#include <string.h>
#include <printk.h>
// drv
#include <vga.h>
#include <kb.h>
#include <ide.h>
// mm
#include <pmm.h>
#include <vmm.h>
#include <heap.h>
// proc
#include <task.h>
#include <sched.h>
// fs
#include <buf.h>
#include <sb.h>
#include <bcache.h>

uint32_t kern_stack_top;
char kern_stack[STACK_SIZE] __attribute__((aligned(16)));

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

    //timer_init(); 

    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);

    pmm_init();
    puts("pmm init...\n\r");
    // pmm_mem_info();
    //pmm_test();
    
    vmm_init();
    puts("vmm init...\n\r");
    // vmm_test();
    
    kb_init();
    puts("kb init...\n\r");

    heap_init();
    puts("heap init...\n\r");
    // heap_test();

    ide_init();
    puts("ide init...\n\r");
    // ide_test();

    bcache_init();
    puts("buffer cache init...\n\r");
    sti();

    fs_test();
    
    for (;;);
    return 0;
}
