#include "types.h"
#include "param.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->pid = 0;
  lk->locked = UNLOCK;
}

void
acquiresleeplock(struct sleeplock *lk)
{
  acquire(&lk->lk);

  // 等待直到该锁没有被持有
  while(lk->locked)
  {
    sleep(lk, &lk->lk);
  }

  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

int holdingsleep(struct sleeplock *lk)
{
  int r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}
