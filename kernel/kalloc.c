#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "spinlock.h"
#include "defs.h"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

extern char end[];

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void *)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
  {
    kfree(p);
  }
}

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa > PHYSTOP)
    panic("kfree");

  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

void *
kalloc(void)
{
  acquire(&kmem.lock);
  struct run *r = kmem.freelist;

  if(r)
  {
    kmem.freelist = r->next;
  }

  release(&kmem.lock);

  if(r)
    memset((char *)r, 5, PGSIZE);

  return (void *)r;
}
