#include <usys.h>
#include <uio.h>
#include <stat.h>

#define CAT_BUF 1024

char buf[CAT_BUF];

/* Arthur2e5: Meow~ */
void kitten(){
    int n;

    for (;;){
        n = _read(stdin, buf, sizeof(buf) - 1);
        if (n <= 0){
            break;
        }
        buf[n] = '\0';
        puts(buf);
    }
}

int main(int argc, char **argv){
    int i, fd = -1;
    struct stat st;

    if (argc > 1){
        /* read from files */
        i = 1;
        while (i < argc){
            _close(0);
            if ((fd = _open(argv[i], O_RONLY)) < 0){
                printf("cat: can not open `%s`\n", argv[i]);
                goto exit;
            }

            if (fd != stdin){
                printf("cat: wrong file descriptor %d", fd);
                goto exit;
            }

            if (_fstat(fd, &st) < 0){
                printf("cat: can not get stat of `%s`\n", argv[i]);
                goto exit;
            }

            if (!S_ISREG(st.mode)){
                printf("cat: `%s` is not a file\n", argv[i]);
                goto exit;
            }

            kitten();

            _close(fd);
            fd = -1;
            i++;
        }
    } else {
        /* read form stdin */
        kitten();
    }

exit:
    if (fd != -1) _close(fd);
    _exit();
}
