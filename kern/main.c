#include <sys.h>
int osmain(void)
{
    init_vga();
    unsigned char a[] = "12345\n";
    puts(a);
    for (;;);
}
