// #define __LOG_ON 1
#include <dev.h>
#include <con.h>
#include <printk.h>

struct dev dtable[NDEV];

void dev_init(){
    con_init(); 
    dtable[DEV_CON].read = con_read;
    dtable[DEV_CON].write = con_write;
}
