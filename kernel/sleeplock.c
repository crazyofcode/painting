#include <types.h>
#include <defs.h>
#include <proc.h>
#include <spinlock.h>
#include <sleeplock.h>

void initsleeplock(struct sleeplock *lk, char *name) {
  initlock(lk->lock, name);
  lk->locked = FALSE;
  lk->name = name;
  lk->pid = -1;
}

void acquiresleeplock(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked) {
    sleep(lk, &lk->lk);
  }
  lk->locked = TRUE;
  lk->pid = getpid();
  release(&lk->lk);
}

void releasesleeplock(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = FALSE;
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