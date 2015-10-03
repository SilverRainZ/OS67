#ifndef __USYSCALL_H
#define __USYSCALL_H

extern int _fork();
extern int _exit();
extern int _wait();
extern int _pipe();
extern int _read();
extern int _kill();
extern int _exec(char *path, char **argv);
extern int _fstat();
extern int _chdir(char *path);
extern int _fdup();
extern int _getpid();
extern int _sbrk();
extern int _sleep();
extern int _uptime();
extern int _open();
extern int _write();
extern int _mknod();
extern int _unlink(char *path);
extern int _link(char *old, char *new);
extern int _mkdir(char *path);
extern int _close();

#endif
