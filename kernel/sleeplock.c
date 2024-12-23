#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <list.h>
#include <macro.h>
#include <defs.h>
#include <proc.h>
#include <sleeplock.h>

void initsleeplock(struct sleeplock *lk, char *name) {
  initlock(&lk->lk, name);
  lk->locked = false;
  lk->name = name;
  lk->pid = -1;
}

void acquiresleeplock(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked) {
    sleep(lk, &lk->lk);
  }
  lk->locked = true;
  lk->pid = getpid();
  release(&lk->lk);
}

void releasesleeplock(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = false;
  lk->pid = -1;
  wakeup(lk);
  release(&lk->lk);
}

bool holdingsleeplock(struct sleeplock *lk)
{
  bool r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == getpid());
  release(&lk->lk);
  return r;
}
