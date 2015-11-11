#ifndef __DEV_H
#define __DEV_H

#include <type.h>
#include <inode.h>

/* device index */
#define DEV_TTY     1

#define NDEV        10

struct dev{
    int (*read)(struct inode*, char*, uint32_t);
    int (*write)(struct inode*, char*, uint32_t);
};

void dev_init();

#endif
