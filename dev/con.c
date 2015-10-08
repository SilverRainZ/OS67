#define __LOG_ON 1
/* con is a device used to user's input & output 
 * input come form keyboard
 * output to screen via vga.c
 */
#include <type.h>
#include <dbg.h>
#include <vga.h>
#include <pipe.h>
#include <file.h>
#include <printk.h>
#include <inode.h>
#include <file.h>

struct file *con_bufin = 0 , *con_bufout = 0;

void con_init(){
    pipe_alloc(&con_bufout, &con_bufin);
}

int con_read(struct inode *ip, char *dest, uint32_t n){
    assert(ip, "con_read: null ip");
    return fread(con_bufout, dest, n);
}

int con_write(struct inode *ip, char *src, uint32_t n){
    uint32_t i;

    assert(ip, "con_read: null ip");

    for (i = 0; i < n; i++){
        putchar(src[i]);
    }

    return n;
}

