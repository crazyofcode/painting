#include "types.h"
#include "timer.h"
#include "sbi.h"
#include "riscv.h"
#include "spinlock.h"
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
  panic("clockintr todo");
}
