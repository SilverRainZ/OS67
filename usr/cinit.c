#include <usys.h>
#include <uio.h>

int main(){
    char *argv[] = {"sh", 0};

    if (uio_init() < 0){
        goto bad;     
    }

    int pid = _fork();
    if (pid == 0){
        if (_exec("/bin/sh", argv) < 0){
            goto bad;
        }
    } else if (pid < 0){
        goto bad;
    }

    if (_wait() < 0) goto bad;
    puts("shell exit.\n");
    for (;;);

bad:    
    puts("Unresolvable fault, check your rootfs.img :(\n");
    /* trigger a genernal protection fault */
    __asm__("int $13"); 
}
