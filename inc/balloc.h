#ifndef __BALLOC_H
#define __BALLOC_H

#include <type.h>
#include <minix.h>

void read_sb(int dev, struct super_block *sb);
void print_sb();
uint32_t balloc(int dev);
void bfree(int dev, uint32_t blkn);

#endif
