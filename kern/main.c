#include <sys.h>
void delay(){   // lol, we have no sleep()
    int i = 0;
    for (i = 0; i < 5000000; i++){
        i++;
        i--;
    }
}

int osmain(void)
{
    vga_init();
    gdt_init();
    puts((unsigned char *)"GDT installed...\n\r\0");
    idt_init();
    puts((unsigned char *)"IDT installed...\n\r\0");
    isrs_init();
    puts((unsigned char *)"ISRs installed...\n\r\0");
    irq_init();
    puts((unsigned char *)"IRQs installed...\n\r\0");

    setcolor(COL_D_GREY, COL_CYAN);
    puts((unsigned char *)"Welcome to OS67...\n\r\0");
    int i;

    for (i = 0; i < 26; i++){
        putchar('A' + i);
        putchar('\n');
        delay();
        if (i == 3) __asm__("int $10"); // ISR test
    }
    //vga_test(); 

    for (;;);
}
