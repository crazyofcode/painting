#ifndef SPINLOCK_H__
#define SPINLOCK_H__

struct spinlock {
  uint32_t    locked;
  struct cpu *cpu;

  char      *name;
};

void initlock(struct spinlock *, char *);
void acquire(struct spinlock *);
void release(struct spinlock *);
void push_off(void);
void pop_off(void);


uint32_t  holding(struct spinlock *);
#endif //!SPINLOCK_H__
