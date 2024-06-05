#include <types.h>
#include <param.h>
#include <riscv.h>
#include <memlayout.h>
#include <spinlock.h>
#include <proc.h>
#include <kalloc.h>
#include <defs.h>
#include <string.h>

struct cpu cpus[NCPU];
struct proc proc[NPROC];

int nextpid = 1;
struct spinlock next_lock;

struct spinlock wait_lock;

void
procinit()
{
  initlock(&next_lock, "next_pid");
  initlock(&wait_lock, "wait_pid");

  struct proc *p;
  for (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((int)(p - proc));
    p->killed = 0;
  }
}

// 通过 inline func 获取 hart id
uint64
cpuid()
{
  uint64 id = r_tp();
  return id;
}

struct cpu *
mycpu()
{
  int id = cpuid();
  return &cpus[id];
}

struct proc *
myproc()
{
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  return p;
}

int
killed(struct proc *c)
{
  acquire(&c->lock);
  if (c->killed) {
    release(&c->lock);
    return 1;
  }

  release(&c->lock);
  return 0;
}

int
either_copyout(int user_dst, void *src, uint64 dst, uint64 len)
{
  struct proc *p = myproc();

  if (user_dst) {
    return copyout(p->pagetable, src, dst, len);
  } else {
    memmove((void *)dst, src, len);
    return 0;
  }

  return -1;
}

int
either_copyin(int user_dst, uint64 src, void * dst, uint64 len)
{
  struct proc *p = myproc();

  if (user_dst) {
    return copyin(p->pagetable, src, dst, len);
  } else {
    memmove(dst, (void *)src, len);
    return 0;
  }
  return -1;
}

void
proc_mapstacks(pagetable_t kpgtbl)
{
  struct proc *p;
  for (p = proc; p < &proc[NPROC]; p++)
  {
    char *pa = kalloc();
    if (pa == NULL)
      panic("out of memory");

    // KSTACK 会分配给每个进程两个 page, 
    // 但是只会映射一个 page,
    // 另一个用于保护进程栈
    uint64 va = KSTACK((int) (p - proc));
    if (mappages(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W))
      panic("proc_mapstacks");
  }
}

