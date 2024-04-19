/*
 * 第一层，盘块I/O
 * 以扇区为单位
*/

#include "include/types.h"
#include "include/param.h"
#include "include/spinlock.h"
#include "include/sleeplock.h"
#include "include/buf.h"
#include "include/riscv.h"
#include "include/defs.h"  
#include "include/disk.h"


/*
 * TODO();
 * sdcard
*/


/* buffer cache */
struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;


/* 
 * * * * * * * * * * * * * * * * * * * * * * 
 * 初始化缓冲区
*/
void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->refcnt = 0;
    b->sectorno = ~0;
    b->dev = ~0;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  /* for test */
  printf("binit\n");

}

// 使用LRU算法, 替换掉最少访问的缓存块
// 寻找一个缓存块给设备号为dev的设备
// 如果没有找到就分配一个
// 无论哪种情况,都返回一个locked buffer
/*
 * * * * * * * * * * * * * * * * * * * * * *
 * 基于 cache 的读写
 * disk_rw 
*/
static struct buf*
bget(uint dev, uint sectorno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->sectorno == sectorno){
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->sectorno = sectorno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}

void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
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

struct buf* 
bread(uint dev, uint sectorno) {
  struct buf *b;

  b = bget(dev, sectorno);
  if (!b->valid) {
    disk_read(b);
    b->valid = 1;
  }

  return b;
}


void 
bwrite(struct buf *b) {
  if(!holdingsleep(&b->lock))
    panic("bwrite");
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