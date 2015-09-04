#define __LOG_ON 1
// std
#include <type.h>
#include <asm.h>
#include <dbg.h>
// x86
#include <pm.h>
#include <isr.h>
#include <timer.h>
#include <syscall.h>
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
// proc
#include <proc.h>
// fs
#include <bcache.h>
#include <inode.h>

int osmain(void)
{
    vga_init();

    gdt_init();
    puts("GDT installed...\n");

    idt_init();
    puts("IDT installed...\n");

    isr_init();
    puts("ISRs installed...\n");

    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n");
    setcolor(COL_L_GREY, COL_BLACK);

    timer_init(); 
    puts("timer init...\n");

    pmm_init();
    puts("pmm init...\n");
    pmm_mem_info();
    // pmm_test();
    
    vmm_init();
    puts("vmm init...\n");
    // vmm_test();
    
    kb_init();
    puts("kb init...\n");

    ide_init();
    puts("ide init...\n");
    // ide_test();

    bcache_init();
    puts("block cache init...\n");

    inode_init();
    puts("inode cache init...\n");

    proc_init();
    puts("porcess init...\n");


    scheduler();
    
    panic("you shouldn't see this :( \n");
    return 67;
}
