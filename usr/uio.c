#include <type.h>
// user libs
#include <usys.h>
#include <uio.h>
// libs
#include <string.h>
#include <vsprint.h>

int uio_init(){
    if (_open("/con", O_RW) < 0){
        if (_mknod("/con", CON) < 0 || _open("/con", O_RW)){
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

char getch(int fd){
    char ch;

    _read(fd, &ch, 1);

    return ch;
}

char getchar(){
    char ch;

    ch = getch(stdin);
    putchar(ch);

    return ch;
}

int _gets(int fd, char *str){
    return _read(fd, str, strlen(str));
}

int gets(char *str){
    return _read(stdin, str, strlen(str));
}
