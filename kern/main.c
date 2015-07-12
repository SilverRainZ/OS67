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
#include <task.h>
#include <sched.h>
#include <ide.h>
#include <buf.h>
#include <fs.h>

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
    pmm_mem_info();
    
    vmm_init();
    puts("vmm init...\n\r");
    vmm_test();
    
    kb_init();
    puts("kb init...\n\r");

    heap_init();
    puts("heap init...\n\r");
    heap_test();

    ide_init();
    puts("ide init...\n\r");

    bcache_init();
    puts("buffer cache init...\n\r");
    sti();

    ide_test();
    show_sb();
    
    for (;;);
    kern_stack_top = (uint32_t)kern_stack + STACK_SIZE;
	__asm__ __volatile__ ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));
    /* 切换新栈 */
    for (;;);
    return 0;
}
