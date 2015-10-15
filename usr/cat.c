#include <usys.h>
#include <uio.h>
#include <stat.h>

#define CAT_BUF 1024

char buf[CAT_BUF];

int main(int argc, char **argv){
    int i, n;
    int fd = -1;
    struct stat st;

    if (argc == 1){
        printf("cat: no support now.\n");
        goto exit;
    }

    if ((fd = _open(argv[1], O_RONLY)) < 0){
        printf("cat: can not open `%s`\n", argv[1]);
        goto exit;
    }

    if (_fstat(fd, &st) < 0){
        printf("cat: can not get stat of `%s`\n", argv[1]);
        goto exit;
    }

    if (!S_ISREG(st.mode)){
        printf("cat: `%s` is not a file\n", argv[1]);
        goto exit;
    }

    for (i = 0; i < st.size; i += CAT_BUF){
        n = CAT_BUF < st.size - i ? CAT_BUF : st.size - i;
        if (_read(fd, buf, n) != n){
            printf("cat: read fault\n");
            goto exit;
        }
        puts(buf);
    }

exit:
    if (fd != -1) _close(fd);
    _exit();
}
