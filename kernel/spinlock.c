#include "param.h"
#include "spinlock.h"
#include "defs.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->locked = unlock;
  lk->name = name;
  lk->cpu = 0;
}

void
acquire(struct spinlock *lk)
{
  push_off();

  if(holding(lk))
    panic("acquire");

  // riscv 提供了一个原子的交换指令
  // __sync_lock_test_and_set()
  while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;

  // 告诉编译器不要交换这的load和store指令
  __sync_synchronize();

  // 保存获取锁的CPU的信息
  lk->cpu = mycpu();
}

void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  // 移出之前保存的cpu信息
  lk->cpu = 0;

  __sync_synchronize();

  // 原子的释放锁
  __sync_lock_release(&lk->locked);

  // 打开中断
  pop_off();
}

// 用于判断锁是否被持有
int
holding(struct spinlock *lk)
{
  int r;

  r = (lk->locked == locked && lk->cpu == mycpu());

  return r;
}

void
push_off()
{
  int old = intr_get();

  intr_off();
  if(mycpu()->noff == 0)
    mycpu()->intena = old;

  mycpu()->noff += ;
}


void
pop_off()
{
  struct cpu *c = mycpu();

  if(intr_get())
    panic("pop_off - interruptible");

  if(c->noff < 1)
    panic("pop_off");

  c->noff -= 1;

  // 当嵌套深度为0,且之前的中断处于打开状态就重新打开中断
  if(c->noff == 0 && c->intena)
    intr_on();
}
