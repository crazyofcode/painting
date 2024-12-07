#include <types.h>
#include <param.h>
#include <macro.h>
#include <spinlock.h>
#include <sleeplock.h>
#include <stdio.h>
#include <list.h>
#include <buf.h>
#include <virt.h>

bcache_t    bcache;
struct list bcache_list;

void binit(void) {
  struct buf* b;

  initlock(&bcache.lock, "bcache");

  // init bcache list
  list_init(&bcache_list);
  for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    initsleeplock(&b->lock, "buffer");
    list_push_back(&bcache_list, &b->elem);
  }
}

// look through buffer cache for block on device dev
// if not found, allocate a buffer
// if either case, return locked buffer
struct buf *bget(uint32_t dev, uint32_t blockno) {
  struct buf *b;
  struct buf *tmp = NULL;

  acquire(&bcache.lock);

  // is the block already cached
  struct list_elem *e;
  for (e = list_begin(&bcache_list); e != list_end(&bcache_list); e = list_next(e)) {
    b = list_entry(e, struct buf, elem);
    if (b->dev == dev && b->blockno == blockno) {
      ++b->refcnt;
      release(&bcache.lock);
      acquiresleeplock(&b->lock);
      return b;
    }
    if (b->refcnt == 0) tmp = tmp == NULL ? b : tmp;
  }

  if (tmp == NULL) {
    panic("bget: no buffer");
  } else {
    tmp->dev = dev;
    tmp->blockno = blockno;
    tmp->valid = 0;
    tmp->refcnt = 1;
    release(&bcache.lock);
    acquiresleeplock(&tmp->lock);
    return tmp;
  }
}

struct buf *bread(uint32_t dev, uint32_t blockno) {
  struct buf *b;

  b = bget(dev, blockno);
  if (!b->valid) {
    virtio_disk_rw(b, READ);
    b->valid = 1;
  }
  return b;
}

void bwrite(struct buf *b) {
  if (!holdingsleeplock(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, WRITE);
}

// release a locked buffer
// move to the head of the most-recently-used list
void brelse(struct buf *b) {
  if (!holdingsleeplock(&b->lock))
    panic("brelse");

  releasesleeplock(&b->lock);

  acquire(&bcache.lock);
  --b->refcnt;
  if (b->refcnt == 0) {
    list_remove(&b->elem);
    list_push_front(&bcache_list, &b->elem);
  }

  release(&bcache.lock);
}

void bpin(struct buf *b) {
  acquire(&bcache.lock);
  ++b->refcnt;
  release(&bcache.lock);
}

void bunpin(struct buf *b) {
  acquire(&bcache.lock);
  --b->refcnt;
  release(&bcache.lock);
}
