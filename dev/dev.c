// #define __LOG_ON 1
#include <dev.h>
#include <tty.h>
#include <printk.h>

struct dev dtable[NDEV];

void dev_init(){
    tty_init(); 
    dtable[DEV_TTY].read = tty_read;
    dtable[DEV_TTY].write = tty_write;
}
