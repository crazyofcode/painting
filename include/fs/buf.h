
#ifndef BUFS_H__
#define BUFS_H__

#ifdef FSTYPE_FAT32
#define BSIZE 512
#endif
#ifdef FSTYPE_EXT4
#define BSIZE 4096
#endif

struct buf {
  int              valid;   // has data been read from disk?
  int              disk;    // does disk "own" buf?
  uint32_t         dev;
  uint32_t         blockno;
  struct sleeplock lock;
  uint32_t         refcnt;
  struct list_elem elem;
  uint8_t          data[BSIZE];
};

typedef struct bcache {
  struct spinlock lock;
  struct buf buf[NBUF];
  struct buf head;
} bcache_t;

void binit();
struct buf *bread(uint32_t, uint32_t);
void bwrite(struct buf *);
struct buf *bget(uint32_t, uint32_t);
void brelse(struct buf *);
void bpin(struct buf *);
void bunpin(struct buf *);

#endif // !BUFS_H__
