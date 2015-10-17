#include <usys.h>
#include <uio.h>
#include <string.h>
#include <stat.h>

/* copy from inc/minix.c */
#define NAME_LEN 14
/* minix directroy entry */
struct dir_entry{
    uint16_t ino;
    char name[NAME_LEN];
};

int main(int argc, char **argv){
    int fd = -1;
    char *dir;
    struct stat st;
    struct dir_entry de;

    if (argc == 1){
        dir = ".";
    } else {
        dir = argv[1];
    }

    if ((fd = _open(dir, O_RONLY)) < 0){
        printf("cat: can not open `%s`\n", argv[1]);
        goto exit;
    }

    if (_fstat(fd, &st) < 0){
        printf("cat: can not get stat of `%s`\n", dir);
        goto exit;
    }

    if (!S_ISDIR(st.mode)){
        printf("cat: `%s` is not a directory\n", dir);
        goto exit;
    }

    while (st.size){
        if (_read(fd, (char *)&de, sizeof(de)) != sizeof(de)){
            printf("cat: read fault\n");
            goto exit;
        }
        if (de.ino != 0){
            puts(de.name);
            putchar('\n');
        }
        st.size -= sizeof(de);
    }

exit:
    if (fd != -1) _close(fd);
    _exit();
}

