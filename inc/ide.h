/* Ref: http://wiki.osdev.org/IDE */
#ifndef __IDE_H
#define __IDE_H
#include <type.h>
/* Disk message */
#define SECTOR_SIZE 512


/* Status */
#define IDE_BSY     0x80    // Busy
#define IDE_DRDY    0x40    // Ready
#define IDE_DF      0x20    // Write fault
#define IDE_ERR     0x01    // Error

/* Command ? */
#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30

/* buf_s -> flag type */
#define B_BUSY  0x1 // buffer is locked
#define B_VALID 0x1 // has been read form disk
#define B_DIRTY 0x4 // need to be written to disk

struct buf_s {
    char flags;
    char dev;
    uint32_t sector;
    char data[512];
};

void ide_init();
void ide_test();

#endif
