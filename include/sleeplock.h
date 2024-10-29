#ifndef SLEEPLOCK_H__
#define SLEEPLOCK_H__

// Long-term locks for processes
struct sleeplock {
  bool            locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

// function
void        initsleeplock(struct sleeplock *, char *);
void        acquiresleeplock(struct sleeplock *);
void        releasesleeplock(struct sleeplock *);
bool        holdingsleeplock(struct sleeplock *);
#endif // !SLEEPLOCK_H__
