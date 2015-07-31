#ifndef __NAMI_H 
#define __NAMI_H

#include <inode.h>

struct inode *nami(char *path);
struct inode *namiparent(char *path, char *name);

#endif
