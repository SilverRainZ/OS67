// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
// mm
#include <pmm.h>
// fs
#include <file.h>
// pipe
#include <pipe.h>

int pipe_alloc(struct file **f0, struct file **f1){
    struct pipe *p;

    p = 0; 
    *f0 = *f1 = 0;
    *f0 = falloc();
    *f1 = falloc();
    assert(*f0 || *f1,"pipe_alloc:");

    p = (struct pipe *)pmm_alloc();

    p->readopen = p->writeopen = 1;
    p->nread = p->nwrite = 0;

    (*f0)->type = F_PIPE;
    (*f0)->readable = 1;
    (*f0)->writeable = 0;
    (*f0)->pipe = p;
    (*f1)->type = F_PIPE;
    (*f1)->readable = 0;
    (*f1)->writeable = 1;
    (*f1)->pipe = p;

    return 0;
}
int pipe_close(struct pipe *p, int writeable){

}

int pipe_write(struct pipe *p, char *addr, int n){

}

int pipe_read(struct pipe *p, char *addr, int n){

}

