#include <usys.h>
#include <uio.h>
#include <stat.h>

#define CAT_BUF 1024

char buf[CAT_BUF];

int main(int argc, char **argv){
    int n;
    int fd = -1;
    struct stat st;

    if (argc > 1){
        _close(0);
        if ((fd = _open(argv[1], O_RONLY)) < 0){
            printf("cat: can not open `%s`\n", argv[1]);
            goto exit;
        }

        if (fd != stdin){
            printf("cat: wrong file descriptor %d", fd);
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
    }

    for (;;){
        n = _read(stdin, buf, sizeof(buf));
        buf[n] = '\0';
        puts(buf);
        if (n != sizeof(buf)){
            break;
        }
    }

exit:
    if (fd != -1) _close(fd);
    _exit();
}
