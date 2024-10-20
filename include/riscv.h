#ifndef RISCV_H__
#define RISCV_H__

#define     SSTATUS_SIE       (1L << 1)

static inline void w_tp(uint64_t x) {
  asm volatile("mv tp, %0" : : "r" (x));
}
static inline uint64_t r_tp() {
  uint64_t x;
  asm volatile("mv %0, tp" : "=r"(x) );
  return x;
}

static inline uint64_t r_satp() {
	uint64_t x;
	asm volatile("csrr %0, satp" : "=r"(x));
	return x;
}

static inline void w_satp(uint64_t x) {
	asm volatile("csrw satp, %0" : : "r"(x));
}

static inline uint64_t r_scause() {
	uint64_t x;
	asm volatile("csrr %0, scause" : "=r"(x));
	return x;
}

static inline uint64_t r_stval() {
	uint64_t x;
	asm volatile("csrr %0, stval" : "=r"(x));
	return x;
}

static inline void  w_stval(uint64_t x) {
  asm volatile("csrw stval, %0" : : "r"(x));
}

static inline uint64_t r_sepc() {
	uint64_t x;
	asm volatile("csrr %0, sepc" : "=r"(x));
	return x;
}

static inline void  w_sepc(uint64_t x) {
  asm volatile("csrw sepc, %0" : : "r"(x));
}
// sstatus register
// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
#define SIE_SPIE (1L << 4)

static inline uint64_t r_sstatus() {
  uint64_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void w_sstatus(uint64_t x) {
  asm volatile("csrw sstatus, %0" : : "r" (x) );
}

/*static inline void */
/*w_sie(uint64_t x)*/
/*{*/
/*  asm volatile("csrw sie, %0" : : "r" (x));*/
/*}*/

/*static inline uint64*/
/*r_sie()*/
/*{*/
/*  uint64_t x;*/
/*  asm volatile("csrr %0, sie" : "=r" (x) );*/
/*  return x;*/
/*}*/


static inline int intr_get() {
  uint64_t sstatus = r_sstatus();

  return (sstatus & SSTATUS_SIE) != 0;
}

// disable interrupt
static inline void intr_on() {
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// enable interrupt
static inline void intr_off() {
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user can access

// flush the TLB.
static inline void
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

// PA PTE
#define   OFFSET          (12)
#define   IDXMASK         (0x1ff)
#define   SHIFT(level)    (OFFSET + level * 9)
#define   PA2PTE(pa)      (((uint64_t)pa >> 12) << 10)
#define   PTE2PA(pte)     (pagetable_t)((pte >> 10) << 12)
#define   IDX(level, va)  (((uint64_t)va >> (SHIFT(level))) & IDXMASK)
#endif  //!RISCV_H__
