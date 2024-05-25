#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

struct spinlock {
  char        *name;
  uint32      lock;
  struct cpu  *cpu;
} ;

void      initlock(struct spinlock *, char *);
void      acquire(struct spinlock *);
void      release(struct spinlock *);

#endif // !__SPINLOCK_H__
