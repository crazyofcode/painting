/*
 * changed by lm.  3.31
 * 双链缓冲区
 * 修改 bread(bio.c)
*/
#ifndef __BUF_H
#define __BUF_H
#define BSIZE 512
#include "sleeplock.h"


// struct buf {
//   int valid;   // has data been read from disk?
//   int disk;    // does disk "own" buf?,  表示该缓冲区的数据已经写到磁盘, 缓冲区中的内容可能会发生变化
//   uint dev;
//   uint blockno;
//   struct sleeplock lock;
//   uint refcnt;      // 引用次数
//   struct buf *prev; // LRU cache list
//   struct buf *next;
//   uchar data[BSIZE];
// };

struct buf {
  int valid;   // 数据是否有效
  int disk;     
  uint dev;
  uint sectorno;  //sector number
  struct sleeplock lock;
  uint refcnt;  // 引用计数，记录当前的缓冲区被引用的次数
  struct buf* prev; //用于双向链表，连接多个缓冲区。
  struct buf* next;
  uchar data[BSIZE];
};

void binit(void);
struct buf* bread(uint, uint);
void brelse(struct buf*);
void bwrite(struct buf*);

#endif