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

// string.c
void *      memset(char *addr, int c, uint size);

// printf.c
void        printfinit();
void        panic(char *);
void        printf(char *, ...);

// console.c
void        consoleinit();
void        consoleputc(int);

// kalloc.c
void        kinit();
void        kfree(void *);
void *      kalloc();

// vm.c
void        kvmmap(pagetable_t, uint64, uint64, uint64, int);
void        kvminit();
int         mappages(pagetable_t, uint64, uint64, uint64, int);
pte_t *     walk(pagetable_t, uint64, int);
void        kvminithart();

// timer.c
void        timerinit();

// bio.c
void        binit();


// spinlock.c
void      initlock(struct spinlock *, char *);
void      acquire(struct spinlock *);
void      release(struct spinlock *);
void      push_off();
void      pop_off();
