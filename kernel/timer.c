#include <types.h>
#include <timer.h>

uint64_t get_timerstamp() {
  uint64_t n;
  asm volatile("rdtime %0" : "=r"(n));
  return n;
}

uint64_t get_time() {
  return get_timerstamp() / CLOCK_PER_USEC;
}

void clock_init() {
  intr_on();
  sbi_set_timer(get_timerstamp() + INTERVAL);
}


void clock_intr() {
  sbi_set_timer(get_timerstamp() + INTERVAL);
}
