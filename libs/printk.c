/* printk.c
 * this file include a useful funcion printk()
 * suppose %d %x %c %s now
 */
//TODO %f doesn't work now, don't use it

// std
#include <type.h>
#include <dbg.h>
// drv
#include <vga.h>
// libs
#include <string.h>
#include <vsprint.h>
#include <printk.h>

void printk(const char *fmt, ...){
    //使用static会给调试带来麻烦
    char buf[256];
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);
    puts(buf);
}

/* this function will output string to host's console 
 * but no display in screen, used for recording log
 */ 

// be called when marco __LOG_ON is defined
void _printl(const char *fmt, ...){
    char buf[256];
    va_list args;
    int i;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);

    //puts(buf);
    for (i = 0; i < strlen(buf); i++){
        bochs_putc(buf[i]);
    }
}
