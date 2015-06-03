void bcache_init();
struct buf* bread(char dev, uint32_t lba);
void bwrite(struct buf* b);
void brelse(struct buf *b);
