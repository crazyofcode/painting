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

static void transtr(uint64_t addr, char *buf, uint64_t size, bool direction) {
  // The implementation of checking the legitimacy of an addr is in the valid_user_arg
  if (direction) {
    if (copyin(cur_proc()->pagetable, buf, addr, size) < 0) {
      log("copy data from user to kernel fault\n");
      process_exit();
    }
  } else {
    if (copyout(cur_proc()->pagetable, addr, buf, size) < 0) {
      log("copy data from user to kernel fault\n");
      process_exit();
    }
  }
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

uint64_t sys_write(void) {
  uint64_t fd;
  uint64_t addr;
  uint64_t size;
  argint(&fd, 0);
  argaddr(&addr, 1);
  argint(&size, &2);

  if (!valid_user_arg(addr)) {
    log("invalid user arg addr: 0x%08x\n", addr);
    return -1;
  } else {
    if (fd == STDOUT || fd == STDERR) {
      char buf[size+1];
      transtr(addr, buf, size, true);
      buf[size] = '\0';
      printf("%s", buf);
    } else {
      size = filesys_write(fd, addr, size);
    }
  }
  return size;
}

uint64_t sys_read(void) {
  uint64_t fd;
  uint64_t addr;
  uint64_t size;
  argint(&fd, 0);
  argaddr(&addr, 1);
  argint(&size, &2);

  if (!valid_user_arg(addr)) {
    log("invalid user arg addr: 0x%08x\n", addr);
    return -1;
  } else {
    char buf[size+1];
    transtr(addr, buf, size, false);
    size = filesys_write(fd, buf, size);
  }
  return size;
}
