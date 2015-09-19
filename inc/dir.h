#ifndef __DIR_H
#define __DIR_H

#include <type.h>
#include <minix.h>

struct inode* dir_lookup(struct inode *dirip, char *name, uint32_t *poff);
int dir_link(struct inode *dirip, char *name, struct inode *fip);
int dir_isempty(struct inode *dip);

#endif
