#include <types.h>
#include <param.h>
#include <riscv.h>
#include <proc.h>
#include <defs.h>

struct cpu cpus[NCPU];
struct proc proc[NPROC];

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
  panic("TODO");
  return 0;
}

int
either_copyin(int user_dst, uint64 src, void * dst, uint64 len)
{
  panic("TODO");
  return -1;
}

void
proc_mapstacks(pagetable_t kpgtbl)
{
  panic("proc_mapstacks TODO");
}
