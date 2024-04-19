/*
 * add function statement
*/

#ifndef __SPINLOCK_H
#define __SPINLOCK_H

// struct cpu;
// 互斥锁
struct spinlock {
  uint locked;
  
  char *name;
  struct cpu *cpu;
};

// Initialize a spinlock 
void initlock(struct spinlock*, char*);

// Acquire the spinlock
// Must be used with release()
void acquire(struct spinlock*);

// Release the spinlock 
// Must be used with acquire()
void release(struct spinlock*);

// Check whether this cpu is holding the lock 
// Interrupts must be off 
int holding(struct spinlock*);

#endif


 