#include <types.h>
#include <param.h>
#include <riscv.h>
#include <memlayout.h>
#include <kalloc.h>
#include <defs.h>
#include <string.h>

extern      char  etext[];      // kernel.ld
extern      char  trampoline[]; // trampoline.S
pagetable_t     kernel_pagetable;

static void
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int term)
{
  if (mappages(kpgtbl, va, pa, sz, term) != 0)
    panic("kvmmap fault");
}

static pagetable_t
kvmmake()
{
  pagetable_t kpgtbl;
  kpgtbl = (pagetable_t)kalloc();

  memset(kpgtbl, 0, PGSIZE);

  // UART
  kvmmap(kpgtbl, VIRT_UART0, VIRT_UART0, PGSIZE, PTE_R | PTE_W);

  // VIRTIO_MMIO
  kvmmap(kpgtbl, VIRTIO, VIRTIO, PGSIZE, PTE_R | PTE_W);

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

void
kvminithart(void)
{
  // wait for all previous write to the page table memory to finish
  sfence_vma();

  w_satp(SATP_MASK(kernel_pagetable));

  // flush the entries from tlb
  sfence_vma();
}

pte_t *
walk(pagetable_t pgtbl, uint64 va, int alloc)
{
  if (va >= MAXVA)
    panic("walk");

  for (int level = 2; level > 0; level--) {
    pte_t *pte = &pgtbl[PX(level, va)];

    if (*pte & PTE_V) {
      pgtbl = (pagetable_t)PTE2PA(*pte);
    } else {
      if (!alloc || (pgtbl = (pde_t *)kalloc()) == NULL)
        return NULL;
      memset(pgtbl, 0, PGSIZE);
      *pte = PA2PTE(pgtbl) | PTE_V;
    }
  }

  return &pgtbl[PX(0, va)];
}

int
mappages(pagetable_t pgtbl, uint64 va, uint64 pa, uint64 sz, int term)
{
  uint64  a = PGROUNDDOWN(va);
  uint64  last = PGROUNDDOWN(va + sz - 1);

  pte_t *pte;
  for (; ; ) {
    if ((pte = walk(pgtbl, a, 1)) == NULL)
      panic("walk");
    if (*pte & PTE_V)
      panic("remap");
    *pte = PA2PTE(pa) | term | PTE_V;

    if (a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  if (va >= MAXVA)
    panic("MAXVA");

  pte_t *pte = walk(pagetable, va, 0);
  if (pte == NULL)
    return 0;
  if (*pte & PTE_V)
    return 0;
  if (*pte & PTE_U)
    return 0;

  uint64 pa = PTE2PA(*pte);

  return pa;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address va in a given page table.
// Return 0 on success, -1 on error.
int
copyin(pagetable_t pagetable, uint64 src, void *dst, uint64 len)
{
  uint64 srcva, pa, n;
  while(len > 0) {
    srcva = PGROUNDDOWN(src);
    pa = walkaddr(pagetable, srcva);

    if (pa == -1)
      return -1;
    n = PGSIZE - (src - srcva);
    if (n > len)
      n = len;
    memmove(dst, (void *)(pa + (src - srcva)), n);

    dst += n;
    len -= n;
    src = srcva + PGSIZE;
  }
  return 0;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
int
copyout(pagetable_t pagetable, void *src, uint64 dst, uint64 len)
{
  uint64 dstva, pa, n;

  while (len > 0) {
    dstva = PGROUNDDOWN(dst);
    pa = walkaddr(pagetable, dstva);

    if (pa == -1) {
      return -1;
    }

    n = PGSIZE - (dst - dstva);
    if (n > len) {
      n = len;
    }

    memmove((void *)(pa + (dst - dstva)), src, n);

    len -= n;
    src += n;
    dst = dstva + PGSIZE;
  }
  return 0;
}

