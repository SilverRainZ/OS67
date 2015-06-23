/* buffer cache 
 * This file is modified form xv6
 */
#include <type.h>
#include <buf.h>
#include <ide.h>
#include <dbg.h>
#include <printk.h>

static struct bcache{
    // spin lock lock   // 待实现的锁
    struct buf buf[NBUF];
   /* head of MRU(Most Recently Used) List, head.next is recently used */
    struct buf head; 
} bcache;

/* create linked list(MRU Cache) of buffer, in reverse */
void bcache_init(){
    struct buf *b;
    bcache.head.prev = &bcache.head;
    bcache.head.next = &bcache.head;
    for (b = bcache.buf; b < bcache.buf + NBUF; b++){
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        b->dev = -1;
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
}

/* look up a NON_BUSY buffer cache by lba and dev 
 * if found set B_BUSY and return
 * else alloc a new buffer
 */
static struct buf* bget(char dev, uint32_t lba){
    struct buf *b;
loop:   
    /* is the sector already cached? */
    for (b = bcache.head.next; b != &bcache.head; b = b->next){
        if (b->dev == dev && b->lba == lba){    // found require buffer
            if ((!b->flags & B_BUSY)){
                b->flags |= B_BUSY;
                return b;
            }
            // if buffer found but B_BUSY, wait and lookup again
            //panic("bget: buffer busy(TEMP)");
            goto loop;
        }
    }

    /* not cached: recycle some non-busy and clean buffer */
    for (b = bcache.head.prev; b != &bcache.head; b = b->prev){
        if ((b->flags & B_BUSY) == 0 && (b->flags & B_DIRTY) == 0){
            b->dev = dev;
            b->lba = lba;
            b->flags = B_BUSY;
            return b;
        }
    }
    panic("bget: no buffers");
    return NULL;
}

/* return a B_BUSY buffer with the content of indicated disk sector */
struct buf* bread(char dev, uint32_t lba){
    struct buf *b;
    b = bget(dev, lba);
    if (!(b->flags & B_VALID)){
        ide_rw(b);
    }
    return b;
}

void bwrite(struct buf* b){
    assert(b->flags & B_BUSY,"bwrite: buffer no busy");
    b->flags |= B_DIRTY;
    ide_rw(b);
}

/* release a B_BUSY buffer, mov it to the haed of MRU list */
void brelse(struct buf *b){
    assert(b->flags & B_BUSY,"bwrite: buffer no busy");

    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;

    b->flags &= ~B_BUSY;
}

