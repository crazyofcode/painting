#ifndef __RISCV_H__
#define __RISCV_H__


#define     SSTATUS_SIE       (1L << 1)


// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software

static inline void 
w_sie(uint64 x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

static inline uint64
r_sie()
{
  uint64 x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline uint64
r_sstatus()
{
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void
w_sstatus(uint64 x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x) );
}

// save hart id
static inline uint64
r_tp()
{
  uint64 x;
  asm volatile("mv %0, tp" : "=r"(x) );
  return x;
}

static inline int
intr_get()
{
  uint64 sstatus = r_sstatus();

  return (sstatus & SSTATUS_SIE) != 0;
}

// disable interrupt
static inline void
intr_on()
{
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// enable interrupt
static inline void
intr_off()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

#define       PTE2PA(pte)       ((((uint64)pte) >> 10) << 12)
#define       PA2PTE(pa)        ((((uint64)pa) >> 12) << 10)

#define       VPN_MASK          0x1FF
#define       OFFSET            12
#define       PX(level, va)     (((uint64)va >> (OFFSET + (level * 9))) & VPN_MASK)

// use riscv's sv39 page table scheme
// page-based 39-bit  virtual addressing
#define       SATP_SV39     (8L << 60)

// use riscv's sv48 page table scheme
// page-based 48-bit  virtual addressing
// #define       SATP_SV48     (9L << 60)

// use riscv's sv57 page table scheme
// page-based 57-bit  virtual addressing
// #define       SATP_SV57     (10 << 60)

#define       SATP_MASK(kernel_pagetable)     (SATP_SV39 | (((uint64)kernel_pagetable) >> 12))

static inline void
sfence_vma()
{
  // zero stand for flush all tlb
  asm volatile("sfence.vma zero, zero");
}

static inline void
w_satp(uint64 x)
{
  asm volatile("csrw  satp, %0" : : "r"(x) );
}

static inline void
w_stvec(uint64 x)
{
  asm volatile("csrw  stvec, %0" : : "r"(x) );
}

#endif // !__RISCV_H__
