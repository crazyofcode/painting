#ifndef __DEFS_H__
#define __DEFS_H__

// proc.c
uint64          cpuid();
struct proc *   myproc();
struct cpu *    mycpu();
int             killed(struct proc *);
int             either_copyin(int, uint64, void *, uint64);
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

#endif // !__DEFS_H__
