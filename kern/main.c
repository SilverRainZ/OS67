#include <sys.h>
#include <const.h>
#include <lib.h>
void delay(){   // lol, we have no sleep()
    int i = 0;
    for (i = 0; i < 1000000; i++){
        i++;
        i--;
    }
}
void test(){
    if (strcmp("1234","1234") == 0) puts("equ");
    else puts("no equ");
}
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

    setcolor(COL_D_GREY, COL_CYAN);
    puts("Welcome to OS67...\n\r");
    setcolor(COL_L_GREY, COL_BLACK);
    //timer_init(); 
    kb_init();
    sti();
    test();
    for (;;);
    int i;
    for (i = 0; i < 26; i++){
        putchar('A' + i);
        putchar('\n');
        delay();
    }
    return 0;
}
