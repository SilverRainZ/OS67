/* ide.c
 * This file is modified form xv6
 * use DMA, assume we hava only a disk
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
    while ((r = inb(0x1f7)) & IDE_BSY){};
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
    if (ide_wait(1) >= 0) insl(0x1f0, buf->data, 512/4);
    //}
    for (i = 0; i < 512; i++){
        printk("%x ",buf->data[i]);
    } 
    printk("\ndone.\n");
    buf->flags |= B_VALID;
    buf->flags &= ~B_DIRTY;
}
void ide_init(){
    /* NB: 一切从简, 不需要检测磁盘的存在, 因为内核已经从磁盘载入了 */
    /* IRQ14 = Primary ATA Hard Disk */
    irq_install_handler(14,ide_handler);
    ide_wait(0);
}

static void ide_start(struct buf_s *buf){
    assert(buf, "idestart: buffer is null");
    ide_wait(0);
    outb(0x3f6, 0);  // generate interrupt
    outb(0x1f2, 1);  // number of sectors
    outb(0x1f3, buf->sector & 0xff);
    outb(0x1f4, (buf->sector >> 8) & 0xff);
    outb(0x1f5, (buf->sector >> 16) & 0xff);
    outb(0x1f6, 0xe0 | ((buf->dev&1)<<4) | ((buf->sector>>24)&0x0f));
    if(buf->flags & B_DIRTY){
        outb(0x1f7, IDE_CMD_WRITE);
        outsl(0x1f0, buf->data, 512/4);
    } else {
        printk("read\n");
        outb(0x1f7, IDE_CMD_READ);
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
