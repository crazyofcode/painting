struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?,  表示该缓冲区的数据已经写到磁盘, 缓冲区中的内容可能会发生变化
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;      // 引用次数
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};
