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
    printf("%d + %d = %d\n\r", 1, 1, 2);
    printf("Wel%s to %s%d...\n\r","come","OS",67);
    printf("Author: %s %s\n\r","LastAvengers","2014-12.24");
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
    int mem = (int)*(short *)0x502;
    printf("Memory: %dMB.\n\r",mem/1024 + 1);
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
