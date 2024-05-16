#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

extern    struct spinlock;

void      initlock(struct spinlock *, char *);
void      acquire(struct spinlock *);
void      release(struct spinlock *);

#endif // !__SPINLOCK_H__
