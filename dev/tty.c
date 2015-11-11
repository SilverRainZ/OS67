// #define __LOG_ON 1
/* tty is a device used to user's input & output 
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
#include <tty.h>
// proc
#include <proc.h>
// fs
#include <inode.h>

struct tty_buf tty_buf;

void tty_init(){
    tty_buf.nread = tty_buf.nwrite = 0;
    memset((void *)tty_buf.buf, 0, NTTY_BUF);
}

/* receive char form kb_hander, write to tty_buf */
int tty_buf_in(char ch){
    if (tty_buf.nwrite == tty_buf.nread + NTTY_BUF){ // full
        if (proc->killed){
            return -1;
        }
        return -1;
    }
    tty_buf.buf[tty_buf.nwrite++ % NTTY_BUF] = ch;
    wakeup(&tty_buf.nread);

    printl("tty_buf_in: receive char [%c] 0x%x, nread: %d nwrite: %d\n", ch, ch, tty_buf.nread, tty_buf.nwrite);
    return 0;
}

int tty_read(struct inode *ip, char *dest, uint32_t n){
    uint32_t ar = n;
    char ch;

    assert(ip, "tty_read: null ip");

    printl("tty_read: inode-%d dest: 0x%x len: %d \n", ip->ino, dest, n);
    while (n > 0){
        if (tty_buf.nread == tty_buf.nwrite){    // empty
            printl("tty_read: empty\n");
            if (proc->killed){
                printl("tty_read: fail\n");
                return -1;
            }
            sleep(&tty_buf.nread);
        }

        ch = tty_buf.buf[tty_buf.nread++ % NTTY_BUF];
        
        if (ch == TTY_EOF){
            break;
        }
        if (ch == TTY_INT){
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

    printl("tty_read: actually read: %d\n", ar - n);
    return ar - n;
}

int tty_write(struct inode *ip, char *src, uint32_t n){
    uint32_t i;

    assert(ip, "tty_read: null ip");

    printl("tty_write: inode-%d src: 0x%x len: %d\n", ip->ino, src, n);

    for (i = 0; i < n; i++){
        putchar(src[i]);
    }

    return n;
}

