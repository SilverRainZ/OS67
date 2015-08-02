/* timer.c 
 * This file is modified form Bram's Kernel Development Tutorial
 * enable programmable interval timer(PIT)
 *
 */
// std
#include <asm.h>
// x86
#include <isr.h>
#include <timer.h>
// drv
#include <vga.h>
#include <printk.h>
// proc
#include <sched.h>

unsigned int timer_ticks = 0;

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

void timer_handler(struct regs_s *r){
    //schedule();
    /* trigger when every 18 clocks (approximately 1 second)  */

    timer_ticks++;
    if (timer_ticks % 18 == 0){
    }
}

/* This will continuously loop until the given time has
 *  been reached */
void timer_wait(int ticks){
    unsigned long eticks;
    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

/* Sets up the system clock by installing the timer handler
 *  into IRQ0 */
void timer_init(){
    irq_install_handler(0, timer_handler);
}
 
