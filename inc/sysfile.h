#ifndef __SYSFILE_H
#define __SYSFILE_H

#define O_RONLY     0x0
#define O_WONLY     0x1
#define O_RW        0x2
#define O_CREATE    0x4

int sys_open();
int sys_write();
int sys_read();
int sys_close();
int sys_fstat();
int sys_mkdir();
int sys_mknod();
int sys_chdir();
int sys_link();
int sys_unlink();

#endif
