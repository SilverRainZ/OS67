// #define __LOG_ON 1
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
// dev
#include <dev.h>

/* 好无聊 */
static void print_ok(){
    int i;
    for (i = 27; i <= 80; i++){
        putchar(' ');
    }

    putchar('[');
    vga_setcolor(COL_L_GREEN, COL_BLACK);
    puts("OK");
    vga_setcolor(COL_L_GREY, COL_BLACK);
    putchar(']');
}

int osmain(void)
{
    vga_init();

    puts("installing GDT...     ");
    gdt_init();
    print_ok();

    puts("installing IDT...     ");
    idt_init();
    print_ok();

    puts("installing ISR...     ");
    isr_init();
    print_ok();

    puts("timer init...         ");
    timer_init(); 
    print_ok();

    puts("pmm init...           ");
    pmm_init();
    pmm_mem_info();
    print_ok();
    // pmm_test();
    
    puts("vmm init...           ");
    vmm_init();
    print_ok();
    // vmm_test();
    
    puts("kb init...            ");
    kb_init();
    print_ok();

    puts("ide init...           ");
    ide_init();
    print_ok();

    // ide_test();
    puts("bcache init...        ");
    bcache_init();
    print_ok();

    puts("icache init...        ");
    inode_init();
    print_ok();

    puts("ftable init...        ");
    file_init();
    print_ok();

    puts("dev init...           ");
    dev_init();
    print_ok();

    puts("porcess init...       ");
    proc_init();
    print_ok();

    scheduler();
    
    panic("you shouldn't see this :( \n");
    return 67;
}
