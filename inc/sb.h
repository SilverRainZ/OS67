#ifndef __SB_H
#define __SB_H

#include <minix.h>

/* super block structure is defined in inc/minix.h */

void read_sb(int dev, struct super_block *sb);
void print_sb();

#endif
