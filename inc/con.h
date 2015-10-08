#ifndef __CON_H
#define __CON_H

#include <type.h>
#include <inode.h>

void con_init();
int con_read(struct inode *ip, char *dest, uint32_t n);
int con_write(struct inode *ip, char *src, uint32_t n);

#endif
