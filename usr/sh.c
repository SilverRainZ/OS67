#include <usys.h>
#include <uio.h>
#include <string.h>

#define LEN_CMD  128
#define MAX_ARGC 20

static int get_cmd(char *cmd){
    printf("$ ");

    return gets(cmd, LEN_CMD);
}

/* splist command 
 * "cat READMD >  cat"  -> "cat", 0, "README", 0, ">", 0, 0, "cat", 0
 * */
static int split_cmd(char *argv[], int *argc, char *cmd){
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

/* parse & execute external command */
/* only support one operator in a command :
 *      cat < README
 *      cat README > newfile
 *      ls | cat
 */
static int parse_cmd(int argc, char **argv){
    int i;
    int fd;
    int pid, pid2;
    int pfd[2];
    char argcmd[LEN_CMD];
    char type;
    char **argv2;

    type = ' ';

    /* need a argument after >, < or | */
    for (i = 1; i < argc - 1; i++){
        if (strcmp("<", argv[i]) == 0
                || strcmp(">", argv[i]) == 0
                || strcmp("|", argv[i]) == 0){

            argc = i;
            argv2 = &argv[i+1];
            type = argv[i][0];
            argv[i] = 0;
            break;
        }
    }

    pid = _fork();
    /********** in new process **********/
    if (pid == 0){
        switch(type){
            case '<':{
                         if ((fd = _open(argv2[0], O_RONLY)) < 0){
                             printf("sh1: can not open file `%s`\n", argv2[0]);
                             _exit();
                         }

                         if (_close(stdin) < 0){
                             puts("sh1: can not close stdin\n");
                             _exit();
                         }

                         if (_dup(fd) < 0){
                             puts("sh1: can not close stdin\n");
                             _exit();
                         }

                         break;
                     }
            case '>':{
                         if ((fd = _open(argv2[0], O_RW | O_CREATE)) < 0){
                             printf("sh1: can not open file `%s`\n", argv2[0]);
                             _exit();
                         }

                         if (_close(stdout) < 0){
                             puts("sh1: can not close stdout\n");
                             _exit();
                         }

                         if (_dup(fd) < 0){
                             puts("sh1: can not close stdin\n");
                             _exit();
                         }
                         break;
                     }
            case '|':{
                         if (_pipe(pfd) < 0){
                             puts("sh1: pipe\n");
                             _exit();
                         }

                         pid2 = _fork();
                         if (pid2 == 0){
                             if (_close(stdin) < 0){
                                 printf("sh2: can not close stdin\n");
                                 _exit();
                             }

                             if (_dup(pfd[0]) != stdin){
                                 printf("sh2: can not dup fd0\n");
                                 _close(pfd[0]);
                                 _exit();
                             }

                             memset(argcmd, 0, sizeof(argcmd));
                             strcpy(argcmd, "/bin/");
                             strcat(argcmd, argv2[0]);

                             if (_exec(argcmd, argv2) < 0){
                                 printf("sh2: can not exec %s\n", argcmd);
                                 _close(pfd[0]);
                                 _close(stdin);
                                 _exit();
                             }
                         } else if (pid2 < 0){
                             printf("sh2: fork fail\n");
                             _close(pfd[0]);
                             _close(stdin);
                             _exit();
                         }
                         break;
                     }
            default:
                     break;
        }


        if (type == '|'){
            if (_close(stdout) < 0){
                _close(pfd[1]);
                _exit();
            }

            if (_dup(pfd[1]) != stdout) {
                _close(pfd[1]);
                _exit();
            }
        }

        memset(argcmd, 0, sizeof(argcmd));
        strcpy(argcmd, "/bin/");
        strcat(argcmd, argv[0]);

        if (_exec(argcmd, argv) < 0){
            /* when type == '|' when argv2 is invaild, printf maybe cause sleeping */
            if (type != '|') printf("sh1: can not exec %s\n", argcmd);
            _close(pfd[1]);
            _close(stdin);
            _exit();
        }
        /**********************************/
    } else if (pid > 0){
        if (_wait() < 0){
            printf("sh: wait %d return -1\n", pid);
            goto bad;
        }
    } else {
        printf("sh: fork fail");
        goto bad;
    }

    return 0;

bad:
    return -1;
}

static void welcome(){
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
    int argc;
    char cmd[LEN_CMD];
    char *argv[MAX_ARGC];

    welcome();
    for (;;){
        argc = 0;
        memset(cmd, 0, sizeof(cmd));
        memset(argv, 0, sizeof(argv));

        if (get_cmd(cmd) < 0) break;

        if (split_cmd(argv, &argc, cmd) < 0){
            printf("sh: split cmd fail, too many arguments\n");
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
            parse_cmd(argc, argv);
        }
    }

    _exit();
}
