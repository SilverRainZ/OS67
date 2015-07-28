#ifndef __INODE_H
#define __INODE_H

#include <type.h>
#include <minix.h>

#define NINODE 500 // length of inodes cache

struct inode* iget(char dev, uint32_t inum);
void iput(struct inode *ip);
void ilock(struct inode *ip);
void iunlock(struct inode *ip);
void iunlockput(struct inode *ip);
int iread (struct inode *ip, char *dest, uint32_t off, uint32_t n);
int iwrite(struct inode *ip, char *src, uint32_t off, uint32_t n);

#endif
