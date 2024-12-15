#ifndef SEMAPHORE_H__
#define SEMAPHORE_H__

struct semaphore {
  unsigned    value;
  struct list waiters;
};

void        sema_init(struct semaphore *, unsigned);
void        sema_down(struct semaphore *);
void        sema_up(struct semaphore *);
#endif  // !SEMAPHORE_H__
