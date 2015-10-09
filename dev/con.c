#define __LOG_ON 1
/* con is a device used to user's input & output 
 * input come form keyboard
 * output to screen via vga.c
 */
// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
#include <string.h>
// drv
#include <vga.h>
// dev
#include <con.h>
// proc
#include <proc.h>
// fs
#include <inode.h>

struct con_buf con_buf;

void con_init(){
    con_buf.nread = con_buf.nwrite = 0;
    memset((void *)con_buf.buf, 0, NCON_BUF);
}

/* receive char form kb_hander, write to con_buf */
int con_buf_in(char ch){
    while (con_buf.nwrite == con_buf.nread + NCON_BUF){ // full
        if (proc->killed){
            return -1;
        }
        wakeup(&con_buf.nread);
        return -1;
    }
    con_buf.buf[con_buf.nwrite++ % NCON_BUF] = ch;
    wakeup(&con_buf.nread);

    printl("con_buf_in: receive char [%c] 0x%x, nread: %d nwrite: %d\n", ch, ch, con_buf.nread, con_buf.nwrite);
    return 0;
}

int con_read(struct inode *ip, char *dest, uint32_t n){
    uint32_t ar = n;
    char ch;

    assert(ip, "con_read: null ip");

    while (n > 0){
        while (con_buf.nread == con_buf.nwrite){    // empty
            if (proc->killed){
                return -1;      // TODO why (?)
            }
            sleep(&con_buf.nread);
        }
        ch = con_buf.buf[con_buf.nread++ % NCON_BUF];

        if (ch == CON_EOF){
            if (n < ar){
                con_buf.nread--;    // TODO (?)
            }
            break;
        }
        *dest++ = ch;
        n--;

        if (ch == '\n'){
            break;
        }
    }

    return ar - n;
}

int con_write(struct inode *ip, char *src, uint32_t n){
    uint32_t i;

    assert(ip, "con_read: null ip");

    for (i = 0; i < n; i++){
        putchar(src[i]);
    }

    return n;
}

