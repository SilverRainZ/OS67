#include <type.h>
// user libs
#include <usys.h>
#include <uio.h>
// libs
#include <string.h>
#include <vsprint.h>

int uio_init(){
    if (_open("/tty", O_RW) < 0){
        if (_mknod("/tty", TTY) < 0 || _open("/tty", O_RW)){
            return -1;
        }
    }
    if (_dup(0) < 0 || _dup(0) < 0){
        return -1;
    }
    return 0;
}

void putch(int fd, char ch){
    _write(fd, &ch, 1);
}

void putchar(char ch){
    putch(stdout, ch);
}

void _puts(int fd, char *str){
    _write(fd, str, strlen(str));
}

void puts(char *str){
    _puts(stdout, str);
}

void fprintf(int fd, char *fmt, ...){
    char buf[256];
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);
    
    _puts(fd, buf);
}

void printf(char *fmt, ...){
    char buf[256];
    va_list args;

    memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    vsprint(buf, fmt, args);
    va_end(args);
    
    puts(buf);
}

char getch(){
    char ch;

    if (_read(stdin, &ch, 1) <= 0) return 0;

    return ch;
}

char getchar(){
    char ch;

    ch = getch();
    if (ch != '\b' && ch != '\0'){
        putchar(ch);
    }

    return ch;
}

/* int gets(char *str) is UNSAFE */
int gets(char *str, uint32_t n){
    int an;

    an = _read(stdin, str, n - 1);   // leave room for '\0'
    if (an < 0) return -1;
    str[an] = '\0';
    return an;
}
