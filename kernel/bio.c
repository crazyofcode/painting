#include "types.h"
#include "sleeplock.h"
#include "buf.h"

struct {
  struct spinlock *lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;

void
binit()
{
  struct buf *b;
  
  initlock(&bcache.lock, "bcache");

}
