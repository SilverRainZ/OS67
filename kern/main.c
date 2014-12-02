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
    init_vga();
    init_gdt();
    puts((unsigned char *)"A new GDT installed...\n\r\0");
    init_idt();
    puts((unsigned char *)"IDT installed...\n\r\0");
    puts((unsigned char *)"Welcome to OS67...\n\r\0");
    //vga_test(); 

    for (;;);
}
