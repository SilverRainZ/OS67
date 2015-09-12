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
// proc
#include <proc.h>

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

void pipe_close(struct pipe *p, int writeable){
    if (writeable){
        p->writeopen = 0;
        wakeup(&p->nread);
    } else {
        p->readopen= 0;
        wakeup(&p->nwrite);
    }

    if (p->readopen == 0 && p->writeopen == 0){
        pmm_free((uint32_t)p);
    }
}

int pipe_write(struct pipe *p, char *addr, int n){
    int i; 
    for (i = 0; i < n; i++){
        while (p->nwrite == p->nread + PIPE_SIZE){   // pipe is full
            if (p->readopen == 0 || proc->killed){
                return -1;
            }
            wakeup(&p->nread);  // wakeup pipe_read
            sleep(&p->nwrite);
        }
        p->data[p->nwrite++ % PIPE_SIZE] = addr[i];
    }

    wakeup(&p->nread);
    return n;
}

int pipe_read(struct pipe *p, char *addr, int n){
    int i;

    while (p->nread == p->nwrite && p->writeopen){  // pile is empty and writeable
        if (proc->killed){
            return -1;
        }
        sleep(&p->nread);
    }
    for (i = 0; i < n; i++){
        if (p->nread == p->nwrite){  // empty
            break;
        }

        addr[i] = p->data[p->nread++ % PIPE_SIZE];
    }
    wakeup(&p->nwrite);

    return i;
}
