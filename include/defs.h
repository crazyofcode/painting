#ifndef __DEFS_H__
#define __DEFS_H__

// proc.c
uint64          cpuid();
struct proc *   myproc();
struct cpu *    mycpu();
int             killed(struct proc *);
int             either_copyin(int, uint64, void *, uint64);
int             either_copyout(int, void *, uint64, uint64);
void            proc_mapstacks(pagetable_t);

// console.c
void            consoleinit();

// printf.c
void            printfinit();
void            printflogo();
void            printf(const char *, ...);
void            panic(const char *);

// vm.c
void            kvminit(void);
void            kvminithart(void);
int             mappages(pagetable_t, uint64, uint64, uint64, int);
int             copyin(pagetable_t, uint64, void *, uint64);
int             copyout(pagetable_t, void *, uint64, uint64);

#endif // !__DEFS_H__
