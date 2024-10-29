#ifndef BUFS_H__
#define BUFS_H__

#define BSIZE     1024
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

#endif // !BUFS_H__
