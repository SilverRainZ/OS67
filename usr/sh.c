#include <usys.h>
#include <uio.h>
#include <string.h>

#define LEN_CMD  128
#define MAX_ARGC 20

char *path  = "/bin/";

int getcmd(char *cmd){
    int len;

    printf("$ ");
    if ((len = gets(cmd, LEN_CMD)) < 0) return -1;
    return len;
}

int parse(char *argv[], int *argc, char *cmd){
    char *pcmd;

    pcmd = cmd;
    while (*pcmd != '\0'){
        if (*pcmd == ' ' || *pcmd == '\t'){
            pcmd++;
            continue; 
        }
        if (*argc >= MAX_ARGC) return -1;
        argv[(*argc)++] = pcmd++;

        while (*pcmd != '\t' && *pcmd != ' ' && *pcmd != '\0') pcmd++;
    }
    argv[*argc] = 0;

    pcmd = cmd;
    while (*pcmd != '\0'){
        if (*pcmd == ' ' || *pcmd == '\t'){
            *pcmd = '\0';
        }
        pcmd++;
    }
    return 1;
}

void welcome(){
    struct stat st;
    int fd;
    char logo[150] = "";

    if ((fd = _open("/share/logo.txt", O_RONLY)) < 0){
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
    if (fd != -1) _close(fd);
}


int main(){
    int argc, pid, len;
    char cmd[LEN_CMD];
    char *argv[MAX_ARGC];
    char argv0[LEN_CMD/2];

    welcome();
    for (;;){
        argc = 0;
        memset(cmd, 0, sizeof(cmd));
        memset(argv, 0, sizeof(argv));

        len = getcmd(cmd);
        if (len < 0) break;

        if (parse(argv, &argc, cmd) < 0){
            printf("parse fail, too many arguments\n");
        };

        if (argc == 0) continue;

        /* build-in command cd */
        if (strcmp(argv[0], "cd") == 0){
            if (argc < 2){
                printf("cd: missing directory.\n");
            } else {
                if (_chdir(argv[1]) < 0){
                    printf("cd: can not change directroy to %s\n", argv[1]);
                }
            }
        /* external command in /bin */
        } else {
            memset(argv0, 0, sizeof(argv0));
            strcpy(argv0, "/bin/");
            strcat(argv0, argv[0]);

            pid = _fork();
            if (pid == 0){
                if (_exec(argv0, argv) < 0){
                    printf("argv: can not exec %s\n", argv0);
                    _exit();
                }
            } else if (pid > 0){
                if (_wait() < 0){
                    printf("sh: wait %d return -1\n", pid);
                    goto bad;
                }
            } else {
                printf("sh: fork fail");
                continue;
            }
        }
    }

bad:
    _exit();
}
