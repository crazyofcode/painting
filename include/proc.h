/*
 * changed by lm. 3.31
 * add fat32
*/

// #ifndef __PROC_H__
// #define __PROC_H__

// // #include "param.h"
// // #include "riscv.h"
// // #include "types.h"
// // #include "spinlock.h"
// // #include "file.h"
// // #include "fat32.h"
// // #include "trap.h"

// // Saved registers for kernel context switches.
// struct context {
//   uint64 ra;
//   uint64 sp;

//   // callee-saved
//   uint64 s0;
//   uint64 s1;
//   uint64 s2;
//   uint64 s3;
//   uint64 s4;
//   uint64 s5;
//   uint64 s6;
//   uint64 s7;
//   uint64 s8;
//   uint64 s9;
//   uint64 s10;
//   uint64 s11;
// };
// struct cpu {
//   struct proc *proc;          // The process running on this cpu, or null.
//   struct context context;     // swtch() here to enter scheduler().
//   int noff;                   // Depth of push_off() nesting.
//   int intena;                 // Were interrupts enabled before push_off()?
// };

// extern struct cpu CPU[NCPU];
// // per-process data for the trap handling code in trampoline.S.
// // sits in a page by itself just under the trampoline page in the
// // user page table. not specially mapped in the kernel page table.
// // uservec in trampoline.S saves user registers in the trapframe,
// // then initializes registers from the trapframe's
// // kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// // usertrapret() and userret in trampoline.S set up
// // the trapframe's kernel_*, restore user registers from the
// // trapframe, switch to the user page table, and enter user space.
// // the trapframe includes callee-saved user registers like s0-s11 because the
// // return-to-user path via usertrapret() doesn't return through
// // the entire kernel call stack.
// struct trapframe {
//   /*   0 */ uint64 kernel_satp;   // kernel page table
//   /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
//   /*  16 */ uint64 kernel_trap;   // usertrap()
//   /*  24 */ uint64 epc;           // saved user program counter
//   /*  32 */ uint64 kernel_hartid; // saved kernel tp
//   /*  40 */ uint64 ra;
//   /*  48 */ uint64 sp;
//   /*  56 */ uint64 gp;
//   /*  64 */ uint64 tp;
//   /*  72 */ uint64 t0;
//   /*  80 */ uint64 t1;
//   /*  88 */ uint64 t2;
//   /*  96 */ uint64 s0;
//   /* 104 */ uint64 s1;
//   /* 112 */ uint64 a0;
//   /* 120 */ uint64 a1;
//   /* 128 */ uint64 a2;
//   /* 136 */ uint64 a3;
//   /* 144 */ uint64 a4;
//   /* 152 */ uint64 a5;
//   /* 160 */ uint64 a6;
//   /* 168 */ uint64 a7;
//   /* 176 */ uint64 s2;
//   /* 184 */ uint64 s3;
//   /* 192 */ uint64 s4;
//   /* 200 */ uint64 s5;
//   /* 208 */ uint64 s6;
//   /* 216 */ uint64 s7;
//   /* 224 */ uint64 s8;
//   /* 232 */ uint64 s9;
//   /* 240 */ uint64 s10;
//   /* 248 */ uint64 s11;
//   /* 256 */ uint64 t3;
//   /* 264 */ uint64 t4;
//   /* 272 */ uint64 t5;
//   /* 280 */ uint64 t6;
// };

// enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// // Per-process state
// struct proc {
//   struct spinlock lock;

//   // struct sigaction sig_table[20];
//   // p->lock must be held when using these:
//   enum procstate state;        // Process state
//   void *chan;                  // If non-zero, sleeping on chan
//   int killed;                  // If non-zero, have been killed
//   int xstate;                  // Exit status to be returned to parent's wait
//   int pid;                     // Process ID

//   // wait_lock must be held when using this:
//   struct proc *parent;         // Parent process

//   // these are private to the process, so p->lock need not be held.
//   uint64 kstack;               // Virtual address of kernel stack
//   uint64 sz;                   // Size of process memory (bytes)
//   pagetable_t pagetable;       // User page table
//   struct trapframe *trapframe; // data page for trampoline.S
//   struct context context;      // swtch() here to run process
//   struct vma *    vma[NOMMAPFILE]; 
//   char            currentDir[MAXPATH];
//   struct file *ofile[NOFILE];  // Open files
//   struct inode *cwd;           // Current directory
//   char name[16];               // Process name (debugging)
//   int  sticks;        // 用户状态下运行的时间
//   int  uticks;        // 内核状态下运行的时间
// };

// #endif // !__PROC_H__
#ifndef __PROC_H
#define __PROC_H

#include "param.h"
#include "riscv.h"
#include "types.h"
#include "spinlock.h"
#include "file.h"
#include "fat32.h"
// #include "trap.h"

// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
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
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  struct proc *parent;         // Parent process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  pagetable_t kpagetable;      // Kernel page table
  // struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct dirent *cwd;          // Current directory
  char name[16];               // Process name (debugging)
  int tmask;                    // trace mask
};

void            reg_info(void);
int             cpuid(void);
void            exit(int);
int             fork(void);
int             growproc(int);
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
int             kill(int);
struct cpu*     mycpu(void);
struct cpu*     getmycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            setproc(struct proc*);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(uint64);
void            wakeup(void*);
void            yield(void);
int             either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
int             either_copyin(void *dst, int user_src, uint64 src, uint64 len);
void            procdump(void);
uint64          procnum(void);
void            test_proc_init(int);

#endif