#include "types.h"
#include "param.h"
#include "fcntl.h"
#include "spinlock.h"
#include "riscv.h"
#include "mmap.h"
#include "proc.h"
#include "defs.h"

struct cpu cpus[NCPU];
struct proc *initproc;

struct spinlock wait_lock;

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
  sched();
  
  release(&p->lock);
  acquire(lk);
}

void
exit(int status)
{
  struct proc *p = myproc();

  if(p == initproc)
    panic("init proc exit");

  // Close all open files.
  for(int fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd]){
      // struct file *f = p->ofile[fd];
      // fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  /*
  begin_op();
  iput(p->cwd);
  end_op();
  p->cwd = 0;
  */

  acquire(&wait_lock);
  // 归还当前目录的inode
  memset(p->currentDir, 0, MAXPATH);
  reparent(p);

  struct vma* vma;

  int i;
  for(i = 0; i < NOMMAPFILE; i++)
  {
    vma = p->vma[i];
    if(vma)
    {
      if(vma->flag & MAP_SHARED) {
        // TODO();
      }
    }
  }

  p->state = ZOMBIE;
  p->xstate = status;

  // wakeup(p->parent);
  acquire(&p->lock);

  release(&wait_lock);

  sched();
  panic("exit");
}

void
sched()
{
  panic("sched todo");
}

void
reparent(struct proc *p)
{
  panic("reparent todo");
}

void
inittasktable()
{
  panic("inittasktable todo");
}

void
initfirsttask()
{
  panic("first task init");
}



void
setkilled(struct proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}


// Give up the CPU for one scheduling round.
void
yield(void)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

void
scheduler()
{
  panic("scheduler todo");
}
