#include <param.h>
#include <types.h>
#include <spinlock.h>
#include <riscv.h>
#include <proc.h>
#include <defs.h>

struct cpu cpus[NCPU];

struct cpu *cur_cpu() {
  uint64_t hartid = r_tp();
  return &cpus[hartid];
}
