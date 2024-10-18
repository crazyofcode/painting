#ifndef RISCV_H__
#define RISCV_H__
#include <types.h>

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

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software

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

static inline uint64_t r_sstatus() {
  uint64_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void w_sstatus(uint64_t x) {
  asm volatile("csrw sstatus, %0" : : "r" (x) );
}


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
#endif  //!RISCV_H__
