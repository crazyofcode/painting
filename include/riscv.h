#ifndef __RISCV_H__
#define __RISCV_H__


#define     SSTATUS_SIE       (1L << 1)


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

#endif // !__RISCV_H__
