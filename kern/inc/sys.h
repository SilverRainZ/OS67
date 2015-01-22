#ifndef __SYSTEM_H
#define __SYSTEM_H

/* keyword "extern" is no necessary */

/* asm.c */
unsigned char inportb(unsigned short _port);
void outportb(unsigned short _port, unsigned char _data);
void cli();
void sti();
void hlt();

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
struct tpoint{
    int x;
    int y;
};
void vga_init();
void cls();
void putchar_at(int x, int y, char ch);
void putchar(char ch);
void puts(char *str);
void setcolor (char b_color, char f_color);
struct tpoint getcur();
void setcur(int x, int y);


/* gdt.c */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_init();

/* idt.c */
void idt_init();
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

/* isrs.c */
// this defines what the stack looks like after an ISR was running
struct regs{
    unsigned int gs, fs, es, ds;      
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp, ss;  
};    
void isrs_init();

/* irq.c */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_init();

/* timer.c */
void timer_init();
void timer_wait();

/* kb.c */
void kb_init();

#endif
