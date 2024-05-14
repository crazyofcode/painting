#include <types.h>
#include <riscv.h>
#include <defs.h>

// 通过 inline func 获取 hart id
uint64
cpuid()
{
  uint64 id = r_tp();
  return id;
}
