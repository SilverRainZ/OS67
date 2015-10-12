#include <usys.h>
#include <uio.h>
#include <string.h>

#define LENCMD  100

int getcmd(char *cmd){
    printf("$ ");
    if (gets(cmd) < 0) return -1;
    return 1;
}

void welcome(){
    struct stat st;
    int fd;
    char logo[150] = "";

    if ((fd = _open("/logo.txt", O_RONLY)) < 0){
        puts("Fail to open /logo.txt\n");
        goto bad;
    }
    if (_fstat(fd, &st) < 0){
        puts("Fail to getstat of /logo.txt\n");
    }
    if (_read(fd, logo, st.size) < 0){
        puts("Fail to read /logo.txt\n");
        goto bad;
    }
    puts(logo);

bad:
    _close(fd);
}

int main(){
    char cmd[LENCMD];


    welcome();
    for (;;){
        memset(cmd, 0, sizeof(cmd));
        if (getcmd(cmd) < 0) break;
        printf("%s: command no found.\n", cmd);
    }
    _exit();
}
