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
    int ino = -1;
    struct stat st;
    struct dir_entry de;

    if ((fd = _open(".", O_RONLY)) < 0){
        printf("pwd: can not open `.`\n");
        goto exit;
    }

    if (_fstat(fd, &st) < 0){
        printf("pwd: can not get stat of `.`\n");
        goto exit;
    }

    while (st.size){
        if (_read(fd, (char *)&de, sizeof(de)) != sizeof(de)){
            printf("pwd: read `.` fault\n");
            goto exit;
        }
        if (strcmp(".", de.name) == 0){
            ino = de.ino;
            break;
        }
        st.size -= sizeof(de);
    }
    
    if (st.size == 0){
        printf("pwd: `.` no found\n");
        goto exit;
    }

    _close(fd);

    if ((fd = _open("..", O_RONLY)) < 0){
        printf("pwd: can not open `..`\n");
        goto exit;
    }

    if (_fstat(fd, &st) < 0){
        printf("pwd: can not get stat of `..`\n");
        goto exit;
    }

    while (st.size){
        if (_read(fd, (char *)&de, sizeof(de)) != sizeof(de)){
            printf("pwd: read `..` fault\n");
            goto exit;
        }
        if (ino == de.ino 
                && strcmp(".", de.name) != 0
                && strcmp("..", de.name) != 0){
            puts(de.name);
            putchar('\n');
            break;
        }
        st.size -= sizeof(de);
    }
    
    if (st.size == 0){
        printf("pwd: `..` no found\n");
        goto exit;
    }

exit:
    if (fd != -1) _close(fd);
    _exit();
}

