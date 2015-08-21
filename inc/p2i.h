#ifndef __P2I_H
#define __P2I_H

#include <inode.h>

struct inode *p2i(char *path);
struct inode *p2ip(char *path, char *name);

#endif
