#include <param.h>
#include <types.h>
#include <list.h>
#include <spinlock.h>
#include <riscv.h>
#include <proc.h>
#include <defs.h>

struct cpu cpus[NCPU];
static struct list process_list;

struct cpu *cur_cpu() {
  uint64_t hartid = r_tp();
  return &cpus[hartid];
}

struct process *process_init() {
  list_init(&process_list);
  struct process *p = malloc(sizeof(struct process));
  list_push_back(&p->elem);
}
