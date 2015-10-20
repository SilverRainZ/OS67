// #define __LOG_ON 1
// std
#include <type.h>
#include <dbg.h>
// libs
#include <printk.h>
// mm
#include <pmm.h>
#include <vmm.h>
// fs
#include <file.h>
// pipe
#include <pipe.h>
// proc
#include <proc.h>

int pipe_alloc(struct file **fr, struct file **fw){
    struct pipe *p;

    p = 0; 
    *fr = *fw = 0;
    *fr = falloc();
    *fw = falloc();
    assert(*fr || *fw,"pipe_alloc:");

    p = (struct pipe *)pmm_alloc();

    p->readopen = p->writeopen = 1;
    p->nread = p->nwrite = 0;

    (*fr)->type = F_PIPE;
    (*fr)->readable = 1;
    (*fr)->writeable = 0;
    (*fr)->pipe = p;
    (*fw)->type = F_PIPE;
    (*fw)->readable = 0;
    (*fw)->writeable = 1;
    (*fw)->pipe = p;

    printl("pipe_alloc: pipe: 0x%x, file read: 0x%x, file write 0x%x\n", p, *fr, *fw);

    return 0;
}

void pipe_close(struct pipe *p, int writeable){
    printl("pipe_close: pipe 0x%x, iswriteable %d\n", p, writeable);
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

    printl("pipe_read: pipe: 0x%x, addr: 0x%x, len: %d\n", p, addr, n);

    while (p->nread == p->nwrite && p->writeopen){  // pile is empty and writeable
        printl("pipe_read: empty, wait\n");

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
