#ifndef __CON_H
#define __CON_H

#include <type.h>
#include <inode.h>

#define NCON_BUF    512

/* console control char */
#define CON_INT     1
#define CON_EOF     2

/* pipe-like sturcture */
struct con_buf{
    char buf[NCON_BUF];
    uint32_t nread;
    uint32_t nwrite;
};

void con_init();
int con_buf_in(char ch);
int con_read(struct inode *ip, char *dest, uint32_t n);
int con_write(struct inode *ip, char *src, uint32_t n);

#endif
