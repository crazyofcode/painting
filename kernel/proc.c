#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"

struct cpu cpus[NCPU];

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
  struct proc *p = c->cpu;
  pop_off();

  return p;
}
