/* ide.c
 * This file is modified form xv6
 * use DMA, assume we hava only a disk
 * 假设系统只有一个ATA磁盘, 并且始终存在
 * 使用LBA28寻址, 使用DMA存取数据
 * 注意磁盘镜像是通过 make fs 生成的rootfs.img文件, 而内核单独存在于floppy.img中
 * 因此你无法通过这里的函数访问到内核.
 */

#include <ide.h> 
#include <asm.h>
#include <type.h>
#include <isr.h>
#include <heap.h>
#include <string.h>
#include <printk.h>
#include <dbg.h>

static struct buf_s *buf;
struct buf_s _buf;

/* wait for disk until it ready, checkerror when checkerr=1 */
int ide_wait(int checkerr){
   // int timeout = 20000;
    int r;
    /* 循环检测直到只有 IDE_DRDY 位被置位 */
    while ((r = inb(IDE_PORT_STATUS)) & IDE_BSY){};
    //assert(timeout == 0,"Wait for disk timeout");

    if (checkerr && (r & (IDE_DF|IDE_ERR))){
        return ERROR;
    }
    return OK;
}

void ide_handler(struct regs_s *r){
    printk("recv ide int\n");
    int i;
    //if ((buf->flags & B_DIRTY) && (ide_wait(1) >= 0)){
    if (ide_wait(1) >= 0) insl(IDE_PORT_DATA, buf->data, 512/4);
    //}
    for (i = 0; i < 512; i++){
        printk("%x ",buf->data[i]);
    } 
    printk("\ndone.\n");
    buf->flags |= B_VALID;
    buf->flags &= ~B_DIRTY;
}
void ide_init(){
    /* NB: 一切从简, 不需要检测磁盘的存在 */
    /* IRQ14 = Primary ATA Hard Disk */
    irq_install_handler(14,ide_handler);
    ide_wait(0);
}

static void ide_start(struct buf_s *buf){
    assert(buf, "idestart: buffer is null");
    ide_wait(0);
    outb(IDE_PORT_ALTSTATUS, 0);  // generate interrupt
    outb(IDE_PORT_SECT_COUNT, 1);  // number of sectors
    outb(IDE_PORT_LBA0, buf->sector & 0xff);
    outb(IDE_PORT_LBA1, (buf->sector >> 8) & 0xff);
    outb(IDE_PORT_LBA2, (buf->sector >> 16) & 0xff);
    /* IDE_PORT_CURRENT = 0x101dhhhh d = driver hhhh = head*/
    /* 0xe0 = 1010000 */
    outb(IDE_PORT_CURRENT, 0xe0 | ((buf->dev&1)<<4) | ((buf->sector>>24)&0x0f)); 
    if(buf->flags & B_DIRTY){
        outb(IDE_PORT_CMD, IDE_CMD_WRITE);
        outsl(IDE_PORT_DATA, buf->data, 512/4);
    } else {
        printk("read\n");
        outb(IDE_PORT_CMD, IDE_CMD_READ);
    }
}

static void ide_rw(struct buf_s *buf){
    assert(!(buf->flags & B_BUSY),"iderw: buffer has been locked.");
    ide_start(buf);
}


void ide_test(){
   buf = &_buf;

  // memset(buf->data, 0, 512);
   int i;
   for (i = 0; i < 512; i++){
       buf->data[i] = 1;
   }
   buf->dev = 0;    // 磁盘从0开始计数
   buf->flags = 0;
   buf->sector = 2;
   ide_rw(buf);
}
