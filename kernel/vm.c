#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

extern char etext[];
extern char trampoline[];

pagetable_t kernel_pagetable;

void
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int perm)
{
  if(mappages(kpgtbl, va, pa, sz, perm) != 0)
    panic("kvmmap");
}

int
mappages(pagetable_t pgtbl, uint64 va, uint64 pa, uint64 sz, int perm)
{
  if(sz == 0)
    panic("mappages: size");

  uint64 a, last;
  pte_t *pte;

  a = PGROUNDDOWN(va);
  last = PGROUNDDOWN(va + sz - 1);

  for(;;)
  {
    if((pte = walk(pgtbl, a, 1)) == 0)
      return -1;
    if(*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;

    if(a == last)
      break;

    a += PGSIZE;
    pa += PGSIZE;
  }

  return 0;
}

void
kvminit()
{
  kernel_pagetable = (pagetable_t)kalloc();
  memset(kernel_pagetable, 0, PGSIZE);

  // uart 寄存器
  kvmmap(kernel_pagetable, UART, UART, PGSIZE, PTE_R | PTE_W);
  // printf("usrt\n");
  // #ifdef QEMU
  // VIRTIO0 mmio 磁盘接口
  kvmmap(kernel_pagetable, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);
  // printf("mmio\n");
  // #endif

  // PLIC
  kvmmap(kernel_pagetable, PLIC, PLIC, 0x4000, PTE_W | PTE_R);
  kvmmap(kernel_pagetable, PLIC + 0x200000, PLIC + 0x200000, 0x4000, PTE_R | PTE_W);
  // printf("plic\n");

  // kernel
  kvmmap(kernel_pagetable, KERNBASE, KERNBASE, (uint64)etext - KERNBASE, PTE_R | PTE_X);
  // printf("kernel\n");

  // 映射剩下的内存
  kvmmap(kernel_pagetable, (uint64)etext, (uint64)etext, PHYSTOP - (uint64)etext, PTE_R | PTE_W);
  // printf("etext\n");

  // trampoline 作为 trap的entry/exit, 需要映射到虚拟地址的顶端
  kvmmap(kernel_pagetable, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);
  // printf("trap\n");
}

void
kvminithart()
{
  sfence_vma();
  w_satp(MAKE_SATP(kernel_pagetable));
  sfence_vma();
}

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
//
// The risc-v Sv39 scheme has three levels of page-table
// pages. A page-table page contains 512 64-bit PTEs.
// A 64-bit virtual address is split into five fields:
//   39..63 -- must be zero.
//   30..38 -- 9 bits of level-2 index.
//   21..29 -- 9 bits of level-1 index.
//   12..20 -- 9 bits of level-0 index.
//    0..11 -- 12 bits of byte offset within the page.
pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc)
{
  if(va >= MAXVA)
    panic("walk");

  for(int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc || (pagetable = (pte_t*)kalloc()) == 0)
        return 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(0, va)];
}
