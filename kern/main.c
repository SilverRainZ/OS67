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

int flag = 1;

int thread(void *arg){
    while (1) {
        if (flag == 1){
            putchar('A');
            flag = 0;
        }
        // TODO 新进程中中断不发生...
    }
    return 0;
}

void thread_test(){
    init_sched();
    kernel_thread(thread, NULL);
    sti();
    while(1){
        if (flag == 0){
            putchar('B');
            flag = 1;
        }
    }
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

    //timer_init(); 

    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);

    pmm_init();
    puts("pmm init...\n\r");
    //pmm_mem_info();
    
    vmm_init();
    puts("vmm init...\n\r");
    //vmm_test();
    
    kb_init();
    //puts("kb init...\n\r");

    heap_init();
    puts("heap init...\n\r");
    //heap_test();

    ide_init();
    puts("ide init...\n\r");

    bcache_init();
    puts("buffer cache init...\n\r");
    sti();
    struct buf *b;
    b = bread(0,2);
    int i;
    for (i = 0; i < 256; i++){
        b->data[i] = 'a';
    }
    bwrite(b);
    ide_print_blk(b);
    //ide_test();
    

    //assert(1 == 2, "test");
    
    for (;;);
    kern_stack_top = (uint32_t)kern_stack + STACK_SIZE;
	__asm__ __volatile__ ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));
    /* 切换新栈 */
    thread_test();
    for (;;);
    return 0;
}
