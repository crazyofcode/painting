#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct cpu cpus[NCPU];

void
cpuinit(uint64 hartid)
{
  cpus[hartid].intena = 0;
  cpus[hartid].noff = 0;
}

int
cpuid()
{
  int id = r_tp();
  return id;
}

struct cpu *
mycpu()
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

int
killed(struct proc *p)
{
  int k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);

  return k;
}

struct proc *
myproc(void)
{
  // 关闭中断 ???
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();

  return p;
}

int
either_copy(int user, uint64 dst, void *src, uint len)
{
  while(1)
    ;
}

void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();

  acquire(&p->lock);
  release(lk);

  p->chan = chan;
  p->state = SLEEPING;

  // 进程切换
  // sched()
  
  release(&p->lock);
  acquire(lk);
}
