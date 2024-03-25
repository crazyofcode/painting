#include "types.h"
#include "times.h"
#include "sbi.h"
#include "riscv.h"
#include "spinlock.h"

struct {
  struct spinlock *lock;
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
