/*dbg.c
 * This file is modified form hurlex 
 * 提供了panic和assert
 * 由于kernel是纯二进制文件， print_stack_trace并没有太大的作用
 */

// std
#include <type.h>
#include <dbg.h> 
#include <asm.h>
// drv
#include <vga.h>
// libs
#include <printk.h>

/* print current runlevel and register */
void print_cur_status(){
    static int round = 0; uint16_t reg1, reg2, reg3, reg4;
    __asm__ __volatile__( 	"mov %%cs, %0;"
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
    setcolor(COL_L_RED, COL_BLACK);
    printk("*** Kernel panic ***\n");
    printk("%s\n", msg);
    printk("Current CPU status:\n");
    print_cur_status();

    //i can't get the name of funciton form call stack, so don't use it
    printk("Call stack:\n");
    print_stack_trace();
    for (;;);
}

/********* Debug functions which depend on bochs ****************/

/* outputs a character to the debug console */
void bochs_putc(char c){
    outb(0xe9, (uint8_t)c);
}

/* stops simulation and breaks into the debug console */
void bochs_break(){
    outw(0x8A00,0x8A00);
    outw(0x8A00,0x08AE0);
}
