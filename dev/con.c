// #define __LOG_ON 1
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
    if (con_buf.nwrite == con_buf.nread + NCON_BUF){ // full
        if (proc->killed){
            return -1;
        }
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

    printl("con_read: inode-%d dest: 0x%x len: %d \n", ip->ino, dest, n);
    while (n > 0){
        if (con_buf.nread == con_buf.nwrite){    // empty
            printl("con_read: empty\n");
            if (proc->killed){
                printl("con_read: fail\n");
                return -1;
            }
            sleep(&con_buf.nread);
        }

        ch = con_buf.buf[con_buf.nread++ % NCON_BUF];
        
        if (ch == CON_EOF){
            break;
        }
        if (ch == CON_INT){
            return -1;
        }

        if (ch == '\b'){
            if (ar - n != 0){
                dest--;
                n++;
                putchar(ch);
            }
            continue;
        }

        putchar(ch);

        if (ch == '\n'){
            break;
        }

        *dest++ = ch;
        n--;
    }

    printl("con_read: actually read: %d\n", ar - n);
    return ar - n;
}

int con_write(struct inode *ip, char *src, uint32_t n){
    uint32_t i;

    assert(ip, "con_read: null ip");

    printl("con_write: inode-%d src: 0x%x len: %d\n", ip->ino, src, n);

    for (i = 0; i < n; i++){
        putchar(src[i]);
    }

    return n;
}

