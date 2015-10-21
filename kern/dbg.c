/* dbg.c
 * This file is modified form hurlex 
 * 提供了 panic 和 assert
 * 由于没有符号表， print_stack_trace并没有太大的作用，还经常引发连锁的 Page Fault
 */

// std
#include <type.h>
#include <dbg.h> 
#include <asm.h>
// drv
#include <vga.h>
// libs
#include <printk.h>
#include <proc.h>

/* print current runlevel and register */
void print_cur_status(){
    static int round = 0; uint16_t reg1, reg2, reg3, reg4;
    __asm__ __volatile__(     "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

    printk("%d: @ring%d\n", round, reg1 & 0x3);
    printk("%d: cs = %x\n", round, reg1);
    printk("%d: ds = %x\n", round, reg2);
    printk("%d: es = %x\n", round, reg3);
    printk("%d: ss = %x\n", round, reg4);
    // round?
    ++round;
}

void print_stack_trace(){
    uint32_t *ebp, *eip;
    __asm__ __volatile__("mov %%ebp, %0":"=r"(ebp));
    while (ebp){
        eip = ebp + 1;
        printk("[0x%x]\n", *eip);
        ebp = (uint32_t *) *ebp;
    }
}

void panic(const char *msg){
    cli();

    vga_setcolor(COL_L_RED, COL_BLACK);

    printk("*** Kernel panic ***\n");

    if (proc){
        printk("proc: `%s`(PID: %d)\n",proc->name, proc->pid);
    }

    printk("%s\n", msg);
    printk("Current CPU status:\n");
    print_cur_status();

    //i can't get the name of funciton form call stack, so don't use it
    printk("Call stack:\n");
    print_stack_trace();
    for (;;) hlt();
}

/********* Debug functions which depend on bochs ****************/

/* outputs a character to the debug console */
void bochs_putc(char c){
    outb(0xe9, (uint8_t)c);
}

