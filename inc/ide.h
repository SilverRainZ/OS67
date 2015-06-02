/* Ref: 
 * http://wiki.osdev.org/IDE
 * http://lxr.oss.org.cn/source/drivers/block/hd.c
 */

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

/* Command */
#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30

/* Port */
#define IDE_PORT_DATA       0x1f0   // word data register (Read-Write)
#define IDE_PORT_ERROR      0x1f1   // byte error register (Read)
#define IDE_PORT_FEATURE    IDE_PORT_ERROR  // byte future register (Write)
#define IDE_PORT_SECT_COUNT 0x1f2   // byte secount0 register (Read-Write)
// we use LBA28 
#define IDE_PORT_LBA0       0x1f3   // byte LAB0 register (Read-Write)
#define IDE_PORT_LBA1       0x1f4   // byte LAB1 register (Read-Write)
#define IDE_PORT_LBA2       0x1f5   // byte LAB2 register (Read-Write)
#define IDE_PORT_CURRENT    0x1f6   // byte 101dhhhh d=drive hhhh=head (Read-Write) TODO(?) 
#define IDE_PORT_STATUS     0x1f7   // byte status register (Read) 
#define IDE_PORT_CMD        IDE_PORT_STATUS // byte status register (Write)
#define IDE_PORT_ALTSTATUS  0x3f6   // same as IDE_PORT_STATUS but doesn't clear IRQ

/* IDE_PORT_STATUS 和 IDE_PORT_ALTSTATUS 
 * */

void ide_init();
void ide_test();

#endif
