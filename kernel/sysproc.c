#include <types.h>
#include <spinlock.h>
#include <list.h>
#include <proc.h>

uint64_t sys_getpid(void) {
  return cur_proc()->pid;
}
