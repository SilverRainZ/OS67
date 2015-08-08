#ifndef __P2I_H
#define __P2I_H

#include <inode.h>

struct inode *path2inode(char *path);
struct inode *path2inode_parent(char *path, char *name);

#endif
