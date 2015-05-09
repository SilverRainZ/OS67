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



char a[] = "1234";
// 当flag = 0 时候， 读出来的值并不是0
int flag = 1;
int thread(void *arg){
    for (;;){
        if (flag == 1){
            printk("A");
            flag = 0;
        }
    }
    return 0;
}
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
    timer_init(); 
    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);
    pmm_init();
    //pmm_mem_info();
    puts("pmm\n\r");
    
    vmm_init();
    //vmm_test();
    puts("vmm\n\r");

    kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE);

	__asm__ __volatile__ ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

    puts("shift stack\n\r");
    kb_init();
    puts("kb\n\r");
    printk("%s flag = =%d\n",a, flag);
    heap_init();
    puts("heap\n\r");
    sti();
    puts("sti\n\r");
    //heap_test();
    sched_init();
    puts("sched\n\r");
    kernel_thread(thread, NULL);
    for (;;){
        if (flag == 1){
            printk("B");
            flag = 1;
        }
    }

    for (;;);
    return 0;
}
