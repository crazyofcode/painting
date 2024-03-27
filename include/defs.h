struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;


// proc.c
int         cpuid();
struct cpu* mycpu();
struct proc*myproc();
int         killed(struct proc *);
void        sleep(void *, struct spinlock *);
int         either_copy(int, uint64, void *, uint);
void        cpuinit(uint64);
void        sched();
void        reparent(struct proc *);
void        inittasktable();
void        initfirsttask();
void        exit(int);
void        setkilled(struct proc *p);
void        yield(void);
void        scheduler();

// string.c
void *      memset(void *addr, int c, uint size);

// printf.c
void        printfinit();
void        panic(char *);
void        printf(char *, ...);

// console.c
void        consoleinit();
void        consoleputc(int);
void        consoleintr(int);

// kalloc.c
void        kinit();
void        kfree(void *);
void        freerange(void *, void *);
void *      kalloc();

// vm.c
void        kvmmap(pagetable_t, uint64, uint64, uint64, int);
void        kvminit();
int         mappages(pagetable_t, uint64, uint64, uint64, int);
pte_t *     walk(pagetable_t, uint64, int);
void        kvminithart();
void        inithartvm();

// timer.c
void        timerinit();
void        setTimeout();
void        clockintr();

// bio.c
void        binit();


// spinlock.c
void      initlock(struct spinlock *, char *);
void      acquire(struct spinlock *);
void      release(struct spinlock *);
void      push_off();
void      pop_off();
int       holding(struct spinlock *);

// sleeplock.c
void      initsleeplock(struct sleeplock *, char *name);
void      acquiresleeplock(struct sleeplock *);
int       holdingsleep(struct sleeplock *);

// trap.c
void      trapinithart();
void      usertrap();
void      usertrapret();
int       devintr();
void      kerneltrap(void);

// plic.c
void      plicinit();
void      plicinithart();
int       irq_claim(void);
void      plic_complete(int);

// virtio.c
void      devinit();
void      virtiointr();

// syscall.c
void      syscall(void);
