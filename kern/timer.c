/* timer.c */
#include <asm.h>
#include <isr.h>
#include <vga.h>
#include <timer.h>
int timer_ticks = 0;

void timer_phase(int hz){ 
    int divisor = 1193180 / hz; 
    /* 计算除数 */    
    outportb(0x43, 0x36); 
    /* 设置指令字节0x36 */    
    outportb(0x40, divisor & 0xFF);
    /* 设置除数的低字节 */    
    outportb(0x40, divisor >> 8);    
    /*设置除数的高字节  */
} 
void timer_handler(struct regs *r){
    timer_ticks++;
    /* Every 18 clocks (approximately 1 second), we will
     *  display a message on the screen */
    if (timer_ticks % 18 == 0)
    {
        struct tpoint p = getcur();
        setcur(78,23);
        putchar('A'+timer_ticks%26);
        setcur(p.x, p.y);
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
 
