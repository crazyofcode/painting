#include <types.h>
#include <param.h>
#include <memlayout.h>
#include <spinlock.h>
#include <defs.h>
#include <string.h>
#include <kalloc.h>

extern char     end[];      // defined by kernel.ld

struct run {
  struct run *next;
} ;

struct {
  struct spinlock lock;
  struct run      *freelist;
} kmem;

static void
freerange(void *pa_start, void *pa_end)
{
  char *p = (char *)PGROUNDUP((uint64)pa_start);

  for(; p + PGSIZE <= (char *)pa_end; p += PGSIZE) {
    kfree(p);
  }
}

void
kmeminit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void *)PHYSTOP);
}

void *
kalloc(void)
{
  struct run *r = NULL;

  acquire(&kmem.lock);

  r = kmem.freelist;
  kmem.freelist = r->next;

  release(&kmem.lock);

  return (void *)r;
}

void
kfree(void *pa)
{
  if ((uint64)pa % PGSIZE != 0 || (uint64)pa < (uint64)end || (uint64)pa > PHYSTOP) {
    panic("kfree");
  }

  // 填充无效值
  memset(pa, 1, PGSIZE);

  struct run *r = (struct run *)pa;

  // 将释放的页面放入空闲链表中
  // 方便下次分配
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}
