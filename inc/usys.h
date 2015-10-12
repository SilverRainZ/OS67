#ifndef __USYSCALL_H
#define __USYSCALL_H

#include <type.h>
#include <stat.h>

#define O_RONLY     0x0
#define O_WONLY     0x1
#define O_RW        0x2
#define O_CREATE    0x4

extern int _fork();
extern int _exit();
extern int _wait();
extern int _pipe();
extern int _read(int fd, char *addr, uint32_t n);
extern int _kill(int pid);
extern int _exec(char *path, char **argv);
extern int _fstat(int fd, struct stat *stat);
extern int _chdir(char *path);
extern int _dup(int fd);
extern int _getpid();
// extern int _sbrk();
extern int _sleep(uint32_t sec);
extern int _uptime();
extern int _open(char *path, uint32_t mode);
extern int _write(int fd, char *addr, uint32_t n);
extern int _mknod(char *path, uint32_t di);
extern int _unlink(char *path);
extern int _link(char *old, char *new);
extern int _mkdir(char *path);
extern int _close(int fd);

#endif
