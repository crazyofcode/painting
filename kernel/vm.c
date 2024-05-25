#include <types.h>
#include <param.h>
#include <memlayout.h>
#include <kalloc.h>
#include <defs.h>
#include <string.h>

extern      char  etext[];      // kernel.ld
extern      char  trampoline[];
pagetable_t     kernel_pagetable;

static void
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int term)
{
  if (1)
    panic("kvmmap TODO");
}

static pagetable_t
kvmmake()
{
  pagetable_t kpgtbl;
  kpgtbl = (pagetable_t)kalloc();

  memset(kpgtbl, 0, PGSIZE);

  // UART
  kvmmap(kpgtbl, VIRT_UART0, VIRT_UART0, PGSIZE, PTE_R | PTE_W);

  // PLIC
  kvmmap(kpgtbl, PLIC, PLIC, 0x400000, PTE_W | PTE_R);

  // etext
  kvmmap(kpgtbl, KERNBASE, KERNBASE, (uint64)etext - KERNBASE, PTE_R | PTE_X);

  // RAM
  kvmmap(kpgtbl, (uint64)etext, (uint64)etext, PHYSTOP - (uint64)etext, PTE_R | PTE_W);

  // trampoline
  kvmmap(kpgtbl, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);

  // 给每个进程分配一个页面作为 process's kernel stack
  proc_mapstacks(kpgtbl);

  return kpgtbl;
}

void
kvminit(void)
{
  // Initialize the one kernel_pagetable
  kernel_pagetable = kvmmake();
}

