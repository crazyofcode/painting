#ifndef __PROC_H__
#define __PROC_H__

struct proc {
  struct spinlock     *lk;
  uint                pid;    // process number
  struct trapframe    *trapframe;    // data page for trampline.S
  struct context      context;       // switch() here to run process, preserve site
  // ... more infomation waiting to add
}

#endif // !__PROC_H__

