/*
 * 将基于 block 的 cache.改为基于 sector 的 cache.
 * BSIZE 在此定义  
*/

#ifndef __BUF_H
#define __BUF_H

#define BSIZE 512 
#include "sleeplock.h"

struct buf {
  int valid;
  int disk;
  uint dev;
  uint sectorno;
  struct sleeplock lock;
  uint refcnt;
  struct buf* prev;
  struct buf* next;
  uchar data[BSIZE];
  
};

/* cache 相关的操作 */
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);
#endif



