#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <list.h>
#include <proc.h>
#include <defs.h>

uint64_t sys_getpid(void) {
  return cur_proc()->pid;
}
