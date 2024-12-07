#include <types.h>
#include <spinlock.h>
#include <proc.h>
#include <vm.h>
#include <syscall.h>

static bool valid_user_arg(uint64_t addr) {
  struct proc *p = cur_proc();
  if (walk(p->pagetable, addr, 0) == NULL)
    return false;
  else
    return true;
}

uint64_t sys_create(void) {
  uint64_t path, mode;
  argaddr(&path, 0);
  argint(&mode, 1);

  if (!valid_user_arg(path)) {
    log("invalid user arg addr: 0x%08x\n", path);
    return -1;
  }

  if (filesys_create(path, mode))
    return -1;
  else
    return 0;
}

uint64_t sys_open(void) {
  uint64_t path, flags;
  argaddr(&path, 0);
  argint(&flags, 1);

  if (!valid_user_arg(path)) {
    log("invalid user arg addr: 0x%08x\n", path);
    return -1;
  } else
    return file_open(path, flags);
}
