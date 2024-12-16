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
#include <trap.h>
#include <macro.h>
#include <stdio.h>
#include <file.h>
#include <buddy.h>
#include <schedule.h>
#include <sbi.h>
#include <vfs.h>

extern char trampoline[];
void swtch(struct context *, struct context *);
struct cpu cpus[NCPU];
unsigned char pid_bitmap[MAX_PID / 8]; // 位图，每个字节包含8个位

static struct spinlock process_lock;
static struct list process_list;
static struct spinlock  pid_lock;

static int _flag2perm(int flag) {
  int perm = 0;
  if (flag & 0x1) perm |= PTE_X;
  if (flag & 0x2) perm |= PTE_W;
  return perm;
}

static bool loadseg(pagetable_t pagetable, int fd, off_t off, uint64_t va, size_t bytes_read, size_t bytes_zero, int flag) {
  ASSERT(((bytes_zero + bytes_read) & PGMASK) == 0);
  ASSERT((va & PGMASK) == 0);
  ASSERT(off % PGSIZE == 0);

  filesys_seek(cur_proc(), fd, off, SEEK_SET);
  while( bytes_read > 0 || bytes_zero > 0 ) {
    size_t page_read_bytes = bytes_read < PGSIZE ? bytes_read : PGSIZE;
    size_t page_zero_bytes = PGSIZE - page_read_bytes;

    void *pa = kpmalloc();
    if (mappages(pagetable, va, (uint64_t)pa, PGSIZE, flag | PTE_R | PTE_U) < 0) {
      kpmfree(pa);
      return false;
    }

    if (filesys_read(cur_proc(), fd, (char *)pa, page_read_bytes) != page_read_bytes) {
      kpmfree(pa);
      return false;
    }

    memset(pa + page_read_bytes, 0, page_zero_bytes);

    bytes_read -= page_read_bytes;
    bytes_zero -= page_zero_bytes;
    va += PGSIZE;
  }

  return true;
}

static bool setup_stack(pagetable_t pagetable, uint64_t frame, uint64_t *sp) {
  void *pa = kpmalloc();
  if (mappages(pagetable, frame, (uint64_t)pa, PGSIZE, PTE_R | PTE_W | PTE_U) < 0) {
    kpmfree(pa);
    return false;
  }
  *sp = frame + PGSIZE;
  return true;
}

struct cpu *cur_cpu(void) {
  uint64_t hartid = r_tp();
  return &cpus[hartid];
}

struct proc* cur_proc(void) {
  struct cpu* c = cur_cpu();
  return c->proc;
}

int getpid() {
  return cur_proc()->pid;
}
// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
// void forkret(void) {
//   // 在scheduler里会获取该进程的锁
//   struct proc *p = cur_proc();
//   release(&p->lock);
//
//   usertrapret();
// }

pagetable_t process_pagetable(struct proc *p) {
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  if(pagetable == 0)
    return 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  if(mappages(pagetable, TRAMPOLINE, (uint64_t)trampoline, PGSIZE, PTE_R | PTE_X) < 0){
    uvmfree(pagetable, 0);
    return 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  if(mappages(pagetable, TRAPFRAME, (uint64_t)(p->trapframe), PGSIZE, PTE_R | PTE_W) < 0){
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}

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
  struct proc *p = (struct proc *)kalloc(sizeof(struct proc), PROC_MODE);
  if (p == NULL) {
    return NULL;
  }
  p->trapframe = (struct trapframe *)kalloc(sizeof(struct trapframe), TRAPFRAME_MODE);
  if (p->trapframe == NULL) {
    kfree(p, PROC_MODE);
    return NULL;
  }

  // alloc kernel stack
  uintptr_t *kstack = (uintptr_t *)kpmalloc();
  if (kstack == NULL) {
    kpmfree(p);
    return NULL;
  }
  p->pid      = pid_alloc();

  uint64_t pa = (uint64_t)kstack;
  uint64_t va = KSTACK(p->pid);
  if (mappages(kernel_pagetable, va, pa, PGSIZE, PTE_R | PTE_W) != 0) {
    kpmfree(kstack);
    kpmfree(p);
    return NULL;
  }
  p->kstack = va;
  // create a user page for the giving process
  p->pagetable = process_pagetable(p);
  // Set up new context to start executing at usertrapret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64_t)usertrapret;
  p->context.sp = p->kstack + PGSIZE;

  // Parameters to initialize the process structure
  p->vruntime = 0;
  p->priority = FIRST;
  p->status   = INITIAL;
  p->cwd      = NULL;
 
  p->parent = cur_proc();     // first proc should be NULL
  list_init(&p->child_list);
  list_init(&p->file_list);

  acquire(&process_lock);
  list_push_back(&process_list, &p->elem);
  release(&process_lock);

  initlock(&p->lock, "proc");

  return p;
}

void run_first_task(void) {
  log("entry first task\n");
  ASSERT(intr_get());
  filesys_init();
  if (process_execute("init", NULL))
    usertrapret();
  sbi_shutdown();
}
bool process_execute(const char *_path, const char *argv[]) {
  uint64_t ustack[MAXARG];
  bool success;
  struct proc *p = cur_proc();
  strncpy(p->name, _path, strlen(_path));
  success = loader(_path);
  if (success) {
    int argc;
    uint64_t sp = p->trapframe->sp;
    uint64_t stackbase = sp - PGSIZE;
    if (argv != NULL) {
      for (argc = 0; argc < MAXARG; argc++) {
        size_t len = strlen(argv[argc]) + 1;
        sp -= len;
        if (sp < stackbase)
          goto bad;
        if (copyout(p->pagetable, sp, argv[argc], len) < 0)
          goto bad;
        ustack[argc] = sp;
      }
      ustack[argc++] = 0;
      uint64_t ptr_size = sizeof(uint64_t);
      uint64_t align_len = (sp & 0x0f) + (0x10 - (argc*ptr_size & 0xf));
      sp -= align_len;
      sp -= argc * ptr_size;
      if (sp < stackbase)
        goto bad;
      if (copyout(p->pagetable, sp, (char *)ustack, argc * ptr_size) < 0)
        goto bad;
    }

    p->trapframe->a1 = sp;
    p->trapframe->a0 = argc-1;
    p->trapframe->sp = sp;
    return true;
  }
bad:
  return false;
}

void process_exit(void) {
  panic("process_exit not implemented");
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
  np->trapframe->a0 = 0;

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

void init_first_proc(void) {
  rb_init();
  struct proc *p = process_create();
  p->context.ra = (uint64_t)run_first_task;
  p->trapframe->a0 = 0;
  p->status = RUNNABLE;
  rb_push_back(p);
}

// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void)
{
  int intena;
  struct proc *p = cur_proc();
  struct cpu *c = cur_cpu();

  if(!holding(&p->lock))
    panic("sched p->lock");
  if(cur_cpu()->noff != 1)
    panic("sched locks");
  if(p->status == RUNNING)
    panic("sched running");
  if(intr_get())
    panic("sched interruptible");

  intena = cur_cpu()->intena;
  swtch(&p->context, &c->context);
  cur_cpu()->intena = intena;
}

void sleep(void *chan, struct spinlock *lk) {
  struct proc *p = cur_proc();

  acquire(&p->lock);
  release(lk);

  p->chan = chan;
  p->status = SLEEPING;
  sched();

  acquire(lk);
  release(&p->lock);
}

void wakeup(void *chan) {
  struct proc *cur = cur_proc();

  // 需要唤醒所有睡在 chan 上的进程
  struct list_elem *e;
  for (e = list_begin(&process_list); e != list_end(&process_list); e = list_next(e)) {
    struct proc *p = list_entry(e, struct proc, elem);
    acquire(&p->lock);
    if (p != cur && p->chan == chan) {
      p->status = RUNNABLE;
      rb_push_back(p);
    }
    release(&p->lock);
  }
}

// programe loader
// Format of an ELF executable file
#define ELF_MAGIC 0x464C457FU

// File header
struct elfhdr {
  uint32_t magic;  // must equal ELF_MAGIC
  uint8_t  elf[12];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint64_t entry;
  uint64_t phoff;
  uint64_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
};

// Program section header
struct proghdr {
  uint32_t type;
  uint32_t flags;
  uint64_t off;
  uint64_t vaddr;
  uint64_t paddr;
  uint64_t filesz;
  uint64_t memsz;
  uint64_t align;
};

// Values for Proghdr type
#define PT_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

bool loader(const char *file) {
  bool success = false;
  uint64_t  sz = 0;
  struct proc *p = cur_proc();
  size_t bytes_zero;
  char path[strlen(file) + 1];
  strncpy(path, file, strlen(file));
  int fd = filesys_open(p, path, 0);
  // 如果打开文件失败就直接返回
  // 通过返回值表示程序是否加载成功
  if (fd < 0)
    return false;

  // 然后读取程序头部信息
  struct elfhdr ehdr;
  ASSERT(filesys_read(p, fd, (char *)&ehdr, sizeof(struct elfhdr)) == sizeof(struct elfhdr));

  #if defined(__ISA_AM_NATIVE__)
  # define EXPECT_TYPE EM_X86_64
  #elif defined(__ISA_X86__)
  # define EXPECT_TYPE EM_386
  #elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
  # define EXPECT_TYPE EM_RISCV
  #elif defined(__ISA_MIPS32__)
  # define EXPECT_TYPE EM_MIPS
  #else
  # error Unsupported ISA
  #endif

  ASSERT(ehdr.magic == ELF_MAGIC);
  ASSERT(ehdr.machine == EXPECT_TYPE);

  struct proghdr phdr[ehdr.phnum];
  filesys_seek(p, fd, ehdr.phoff, SEEK_SET);
  size_t bytes_read = filesys_read(p, fd, (char *)phdr, sizeof (struct proghdr) * ehdr.phnum);
  ASSERT(bytes_read == sizeof(struct proghdr) * ehdr.phnum);

  for (int i = 0; i < ehdr.phnum; i++) {
    if(phdr[i].type != PT_LOAD)
      continue;
    if(phdr[i].memsz < phdr[i].filesz)
      goto done;
    if(phdr[i].vaddr + phdr[i].memsz < phdr[i].vaddr)
      goto done;

    size_t page_offset = phdr[i].vaddr & PGMASK;
    uint64_t  file_page = phdr[i].off & ~PGMASK;
    uint64_t  mem_page  = phdr[i].vaddr & ~PGMASK;
    if (phdr[i].filesz > 0) {
      bytes_read = page_offset + phdr[i].filesz;
      bytes_zero = PGROUNDUP(page_offset + phdr[i].memsz) - bytes_read;
    } else {
      bytes_read = 0;
      bytes_zero = PGROUNDUP(page_offset + phdr[i].memsz);
    }

    if (!loadseg(p->pagetable, fd, file_page, mem_page, bytes_read, bytes_zero, _flag2perm(phdr[i].flags)))
      goto done;
    sz = mem_page + bytes_read + bytes_zero;
  }
  filesys_close(p, fd);
  if (!setup_stack(p->pagetable, sz, &p->trapframe->sp))
    goto done;
  p->trapframe->epc = ehdr.entry;
  success = true;

done:
  return success;
}
