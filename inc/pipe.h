#ifndef __PIPE_H
#define __PIPE_H

#include <type.h>
#include <file.h>

#define PIPE_SIZE 512

struct pipe {
    char data[PIPE_SIZE];
    uint32_t nread;
    uint32_t nwrite;
    int readopen;
    int writeopen;
};

int pipe_alloc(struct file **f0, struct file **f1);
int pipe_close(struct pipe *p, int writeable);
int pipe_write(struct pipe *p, char *addr, int n);
int pipe_read(struct pipe *p, char *addr, int n);

#endif
