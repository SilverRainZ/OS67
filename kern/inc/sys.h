#ifndef __SYSTEM_H
#define __SYSTEM_H

/* vga.c */
#define COL_BLACK   0
#define COL_BLUE    1
#define COL_GREEN   2
#define COL_CYAN    3
#define COL_RED     4
#define COL_MAGENTA 5
#define COL_BROWN   6
#define COL_L_GREY  7
#define COL_D_GREY  8
#define COL_L_BLUE  9
#define COL_L_GREEN 10
#define COL_L_CYAN  11
#define COL_L_RED   12
#define COL_L_MAG   13
#define COL_L_BROWN 14
#define COL_WHITE   15
// L_ == Light D_ == Dark

extern void vga_init();
extern void cls();
extern void putchar(char ch);
extern void puts(char *str);
extern void setcolor (char b_color, char f_color);
extern void vga_test();

/* func.c */
extern void *memcpy(void *dest, const void *src, int count);
extern void *memset(void *dest, char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(const char *str);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);

/* gdt.c */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_init();

/* idt.c */
extern void idt_init();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);


/* isrs.c */
// this defines what the stack looks like after an ISR was running
struct regs{
    unsigned int gs, fs, es, ds;      
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp, ss;  
};    
extern void isrs_init();

/* irq.c */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_init();

/* timer.c */
extern void timer_init();
extern void timer_wait();
#endif
