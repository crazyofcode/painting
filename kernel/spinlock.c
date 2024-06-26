#include <types.h>
#include <param.h>
#include <riscv.h>
#include <spinlock.h>
#include <proc.h>
#include <defs.h>

#define HOLDING(lk)       (lk->lock == LOCK && lk->cpu == mycpu())

void
initlock(struct spinlock *lk, char *name) {
  lk->name = name;
  lk->lock = UNLOCK;
  lk->cpu = 0;
}

// 关闭中断
static void
push_off() {
  // get current sstatus SIE's value
  int old = intr_get();

  intr_off();

  struct cpu *c = mycpu();
  if(c->noff == 0)
    c->intena = old;

  ++c->noff;
}

static void
pop_off() {
  struct cpu *c = mycpu();
  // check the interrupt state
  if (intr_get())
    panic("pop_off - interruptible");

  if (c->noff < 1)
    panic("pop_off - noff has been set to zero");
  --c->noff;
  if (c->noff == 1 && c->intena)
    intr_on();
}

// 获取自旋锁
void
acquire(struct spinlock *lk) {
  push_off();

  if (HOLDING(lk))
    panic("acquire");
  // On RISC-V, sync_lock_test_and_set turns into an atomic swap:
  //   a5 = 1
  //   s1 = &lk->locked
  //   amoswap.w.aq a5, a5, (s1)
  while(__sync_lock_test_and_set(&lk->lock, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen strictly after the lock is acquired.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize();

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu();
}

// 释放自旋锁
void
release(struct spinlock *lk) {
  // 判断 lk's state == unavailable
  if (!HOLDING(lk)) {
    panic("release!");
  }

  lk->cpu = 0;
  // Tell the C compiler and the CPU to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other CPUs before the lock is released,
  // and that loads in the critical section occur strictly before
  // the lock is released.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code doesn't use a C assignment, since the C standard
  // implies that an assignment might be implemented with
  // multiple store instructions.
  // On RISC-V, sync_lock_release turns into an atomic swap:
  //   s1 = &lk->locked
  //   amoswap.w zero, zero, (s1)
  __sync_lock_release(&lk->lock);

  pop_off();
}

