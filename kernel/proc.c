#include <param.h>
#include <types.h>
#include <memlayout.h>
#include <list.h>
#include <spinlock.h>
#include <riscv.h>
#include <proc.h>
#include <defs.h>
#include <vm.h>
#include <pm.h>
#include <string.h>

struct cpu cpus[NCPU];
unsigned char pid_bitmap[MAX_PID / 8]; // 位图，每个字节包含8个位

static struct spinlock process_lock;
static struct list process_list;
static struct spinlock  pid_lock;

struct cpu *cur_cpu(void) {
  uint64_t hartid = r_tp();
  return &cpus[hartid];
}

struct proc* cur_proc(void) {
  struct cpu* c = cur_cpu();
  return c->proc;
}

void forkret(void) {
  return;
}

pagetable_t process_pagetable(struct proc *p) {return NULL;};
void process_init(void) {
  // 初始化保存 proc 的链表
  list_init(&process_list);
  // 初始化进程链表的锁
  initlock(&process_lock, "process_lock");
  // 初始化进程号的🔒
  initlock(&pid_lock, "pid_lock");
}

// free pid
void pid_free(pid_t pid) {
  acquire(&pid_lock);
  if (pid >= 0 && pid < MAX_PID) {
      pid_bitmap[pid / 8] &= ~(1 << (pid % 8)); // 释放该PID号，将该位设为0
  }
  release(&pid_lock);
}

// alloc pid
pid_t pid_alloc(void) {
  acquire(&pid_lock);
  for (pid_t i = 0; i < MAX_PID; i++) {
      if ((pid_bitmap[i / 8] & (1 << (i % 8))) == 0) {
          pid_bitmap[i / 8] |= (1 << (i % 8)); // 设置该位为1，表示已分配
          release(&pid_lock);
          return i;
      }
  }
  release(&pid_lock);
  return -1; // 没有可用的PID
}

struct proc *process_create(void) {
  // create a process
  // first alloc a page for save the information of process
  struct proc *p = (struct proc *)kpmalloc();
  if (p == NULL) {
    return NULL;
  }

  // alloc kernel stack
  uintptr_t *kstack = (uintptr_t *)kpmalloc();
  if (kstack == NULL) {
    kpmfree(p);
    return NULL;
  }
  uint64_t pa = (uint64_t)kstack;
  uint64_t va = KSTACK(list_size(&process_list));
  if (mappages(kernel_pagetable, va, pa, PGSIZE, PTE_R | PTE_W) != 0) {
    kpmfree(kstack);
    kpmfree(p);
    return NULL;
  }
  p->kstack = va;
  // create a user page for the giving process
  p->pagetable = process_pagetable(p);
  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64_t)forkret;
  p->context.sp = p->kstack + PGSIZE;

  // Parameters to initialize the process structure
  p->vruntime = 0;
  p->priority = FIRST;
  p->status   = INITIAL;
  p->pid      = pid_alloc();
  
  acquire(&process_lock);
  list_push_back(&process_list, &p->elem);
  release(&process_lock);

  return p;
}

pid_t fork(void) {
  pid_t pid;
  struct proc *np, *p;

  p = cur_proc();
  if ((np = process_create()) == NULL) {
    return -1;
  }

  // copy on write
  // set flag

  //copy user register
  memcpy((void *)&np->trapframe, (void *)&p->trapframe, sizeof(p->trapframe));
  // set the child process return value is zero
  np->trapframe.a0 = 0;

  // copy open file descriptor
  // TODO

  strncpy(np->name, p->name, sizeof(p->name));
  pid = np->pid;

  acquire(&np->lock);
  np->status = RUNNABLE;
  np->parent = p;
  release(&np->lock);

  return pid;
}

void run_first_proc(void) {
  // run sh
}
