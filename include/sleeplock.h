/*
 * changed by lm. 3.31
*/

#ifndef __SLEEPLOCK_H
#define __SLEEPLOCK_H

#include "types.h"
#include "spinlock.h"   //定义了自旋锁相关的结构和函数，用于实现线程同步和互斥访问。

struct spinlock;

// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

void acquiresleeplock(struct sleeplock*);
void releasesleep(struct sleeplock*);
void initsleeplock(struct sleeplock*,char*);
int holdingsleep(struct sleeplock*);

#endif