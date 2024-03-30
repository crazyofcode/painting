#include "types.h"
#include "param.h"
#include "timer.h"
#include "sbi.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

int ticks;

struct {
  struct spinlock lock;
  int ticks;
} timer;

void
timerinit()
{
  initlock(&timer.lock, "times");
  ticks = 0;
  setTimeout();
}

void
setTimeout()
{
  int t = r_time() + intervel;
  sbi_set_timer(t);
}

void
clockintr()
{
  acquire(&timer.lock);
  ++ticks;

  if(ticks == 60 * 10)
    panic("run out of time");
  wakeup(&ticks);
  release(&timer.lock);

  struct proc *p = myproc();

  if(p == 0)
  {
    setTimeout();
    return;
  }

  if((r_sstatus() & SSTATUS_SPP) == 0)        // user
  {
    acquire(&p->lock);
    ++p->uticks;
    release(&p->lock);
  } else {          // kernel
    acquire(&p->lock);
    ++p->sticks;
    release(&p->lock);
  }
  setTimeout();
  // panic("clockintr todo");
}
