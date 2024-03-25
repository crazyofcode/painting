#include "memlayout.h"
#include "spinlock.h"

struct run {
  struct run *next;
}

struct {
  struct spinlock *lock;
  struct run freelist;
} kmem;

extern char end[];

void
kinit()
{
  initlock(&kmem.lock, kmem);
  freerange(end, (void *)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for(; p < pa_end; p += PGSIZE)
  {
    kfree(p);
  }
}

void
kfree(void *pa)
{
  struct run *r;

  if((pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa > PHYSTOP)
    panic("kfree");

  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&pmem.lock);
  r = pmem.freelist->next;
  pmem.freelist = r;
  release(&pmem.lock);
}

void *
kalloc(void)
{
  acquire(&pmem.lock);
  struct run *r = pmem.lock;

  if(r)
  {
    pmem.freelist = r->next;
  }

  release(&pmem.lock);

  if(r)
    memset((char *)r, 5, PGSIZE);

  return (void *)r;
}
