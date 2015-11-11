#ifndef __TTY_H
#define __TTY_H

#include <type.h>
#include <inode.h>

#define NTTY_BUF    512

/* console control char */
#define TTY_EOF     254
#define TTY_INT     255

/* pipe-like sturcture */
struct tty_buf{
    char buf[NTTY_BUF];
    uint32_t nread;
    uint32_t nwrite;
};

void tty_init();
int tty_buf_in(char ch);
int tty_read(struct inode *ip, char *dest, uint32_t n);
int tty_write(struct inode *ip, char *src, uint32_t n);

#endif
