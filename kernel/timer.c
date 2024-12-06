#include <types.h>
#include <riscv.h>
#include <timer.h>
#include <sbi.h>
#include <macro.h>

uint64_t get_timerstamp() {
  uint64_t n;
  asm volatile("rdtime %0" : "=r"(n));
  return n;
}

uint64_t get_time() {
  return get_timerstamp() / CLOCK_PER_USEC;
}

void clock_init() {
  uint64_t ticks = get_timerstamp() + INTERVAL;
  sbi_set_timer(ticks);
}


void clock_intr() {
  sbi_set_timer(get_timerstamp() + INTERVAL);
}
