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
    puts((unsigned char *)"Welcome to OS67.\n\r\0");
    setcolor(COL_CYAN, COL_BROWN);
    puts((unsigned char *)"Welcome to OS67.\n\r\0");
    int i;
    for (i = 0; i < 28; i++){
        putchar(i + '0');
        putchar('\n');
        delay();
    }
    //vga_test(); 
    
    for (;;);
}
