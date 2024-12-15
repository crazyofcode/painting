#include <types.h>
#include <param.h>
#include <list.h>
#include <macro.h>
#include <spinlock.h>
#include <proc.h>
#include <defs.h>
#include <schedule.h>
#include <semaphore.h>

void sema_init(struct semaphore *sema, unsigned value) {
  sema->value = value;
  list_init(&sema->waiters);
}

void sema_down(struct semaphore *sema) {
  push_off();
  ASSERT(sema);

  while (sema->value == 0) {
    list_push_back(&sema->waiters, &cur_proc()->elem);
    yield();
  }

  sema->value--;
  __sync_synchronize();
  pop_off();
}

void sema_up(struct semaphore * sema) {
  push_off();
  ASSERT(sema);

  if (!list_empty(&sema->waiters)) {
    struct list_elem *e = list_pop_front(&sema->waiters);
    struct proc *p      = list_entry(e, struct proc, elem);
    wakeup(p);
  }
  sema->value++;
  __sync_synchronize();

  pop_off();
}
