#ifndef __PROC_H__
#define __PROC_H__

struct context {
  uint64 ra;
  uint64 sp;

  // context swtch with no need for save all register
  // save all callee-save register
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
} ;

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
// enum PRIORITY  { FIRST = 1, SECOND, THIRD, FORTH, FIFTH};

struct proc {
  struct spinlock     lock;
  uint32              pid;    // process number
  struct trapframe    *trapframe;    // data page for trampline.S
  struct context      context;       // switch() here to run process, preserve site

  int                 killed;         // if not zero , have been killed
  pagetable_t         pagetable;
  enum procstate      state;
  uint64              kstack;

  void *              chan;
  // uint32              priority;
  // ... more infomation waiting to add
} ;

struct cpu {
  struct proc *proc;          // process infomation
  struct context context;     // process context
  int     noff;               // number of nested layers
  int     intena;             // old interrput state
} ;
#endif // !__PROC_H__

