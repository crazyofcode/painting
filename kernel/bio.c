/*
 * changed by lm 3.31
 * bread
*/

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"
#include "riscv.h"
#include "defs.h"

#include "printf.h"
#include "disk.h"

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;

void
binit()
{
  // 将扇区号 sectorno 和设备号 dev 初始化为最大值。
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;

  for(b = bcache.buf; b < bcache.buf + NBUF; b++)
  {
    b->refcnt = 0;
    b->sectorno = ~0;
    b->dev = ~0;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    // 初始化缓冲区中NBUF个struct buf所对应的睡眠锁
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  /* test */
  printf("bioinit is ok!\n");
}

// 使用LRU算法, 替换掉最少访问的缓存块
// 寻找一个缓存块给设备号为dev的设备
// 如果没有找到就分配一个
// 无论哪种情况,都返回一个locked buffer
static struct buf*
bget(uint dev, uint sectorno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // 是否已经在缓冲区
  for(b = bcache.head.next; b != &bcache.head; b = b->next)
  {
    if(b->dev == dev && b->sectorno == sectorno)  //替换为扇区号
    {
      ++b->refcnt;
      release(&bcache.lock);
      acquiresleeplock(&b->lock);
      return b;
    }
  }

  // 没有找到为该设备号的而设备分配一块缓冲区
  for(b = bcache.head.next; b != &bcache.head; b = b->next)
  {
    if(b->refcnt == 0)
    {
      b->dev = dev;
      b->sectorno = sectorno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleeplock(&b->lock);
      return b;
    }
  }

  panic("bget: buffer");

  // 不会到达这里
  return 0;
}

void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  --b->refcnt;

  // 该缓冲区已经没有被使用,就会被回收
  if(b->refcnt == 0)
  {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }

  release(&bcache.lock);
}

// 返回一个锁定的sector
struct buf *
bread(uint dev, uint sectorno)
{
  struct buf *b;
  // acquire(&bcache.lock);

  b = bget(dev, sectorno);
  if(!b->valid){
    disk_read(b);
    b->valid = 1;
  }
  return b;
}

void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");

  // TODO()
  disk_write(b);
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}