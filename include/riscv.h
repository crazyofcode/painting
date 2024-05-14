#ifndef __RISCV_H__
#define __RISCV_H__

// 用于保存 hart id
static inline uint64
r_tp()
{
  uint64 x;
  asm volatile("mv %0, tp" : "=r"(x) );
  return x;
}

#endif // !__RISCV_H__
