// #define __LOG_ON 1
/* timer.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * enable programmable interval timer(PIT)
 *
 */
// std
#include <asm.h>
#include <dbg.h>
// x86
#include <pm.h>
#include <isr.h>
#include <timer.h>
// drv
#include <vga.h>
#include <printk.h>
// proc 
#include <proc.h>

uint32_t timer_ticks = 0;

void timer_phase(int hz){ 
    int divisor = 1193180 / hz; 
    /* 计算除数 */    
    outb(PIT_MOD_CMD, 0x36); 
    /* 设置指令字节0x36 */    
    outb(PIT_CH0, divisor & 0xFF);
    /* 设置除数的低字节 */    
    outb(PIT_CH0, divisor >> 8);    
    /*设置除数的高字节  */
} 

void timer_handler(struct int_frame *r){
    timer_ticks++;

    if (!proc) return;

    if (proc->killed && (proc->fm->cs & 0x3) == CPL_USER){
        exit();
    }

    wakeup(&timer_ticks);

    if (timer_ticks % 100 == 0){
        printl("timer_handler: timer alive, trick: %d\n", timer_ticks);
    }

    sched();
}

/* Sets up the system clock by installing the timer handler
 *  into IRQ0 */
void timer_init(){
    timer_phase(100);   // tigger 100 times pre second
    irq_install(IRQ_TIMER, timer_handler);
}
 
