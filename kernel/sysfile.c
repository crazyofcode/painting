#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <macro.h>
#include <stdio.h>
#include <list.h>
#include <proc.h>
#include <defs.h>
#include <vm.h>
#include <vfs.h>
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
      process_exit(-1);
    }
  } else {
    if (copyout(cur_proc()->pagetable, addr, buf, size) < 0) {
      log("copy data from user to kernel fault\n");
      process_exit(-1);
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

  char filename[MAX_FILE_NAME_LEN];
  transtr(path, filename, MAX_FILE_NAME_LEN, true);
  if (filesys_create(cur_proc(), filename, mode))
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
  } else {
    char filename[MAX_FILE_NAME_LEN];
    transtr(path, filename, MAX_FILE_NAME_LEN, true);
    return filesys_open(cur_proc(), filename, flags);
  }
}

uint64_t sys_write(void) {
  uint64_t fd;
  uint64_t addr;
  uint64_t size;
  argint(&fd, 0);
  argaddr(&addr, 1);
  argint(&size, 2);

  if (!valid_user_arg(addr)) {
    log("invalid user arg addr: 0x%08x\n", addr);
    return -1;
  } else {
    char buf[size+1];
    transtr(addr, buf, size, true);
    if (fd == STDOUT || fd == STDERR) {
      buf[size] = '\0';
      printf("%s", buf);
    } else {
      size = filesys_write(cur_proc(), fd, buf, size);
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
  argint(&size, 2);

  if (!valid_user_arg(addr)) {
    log("invalid user arg addr: 0x%08x\n", addr);
    return -1;
  } else {
    char buf[size+1];
    size = filesys_read(cur_proc(), fd, buf, size);
    transtr(addr, buf, size, false);
  }
  return size;
}

uint64_t sys_close(void) {
  uint64_t fd;
  argint(&fd, 0);

  if (fd < 2)
    return -1;
  else
    return filesys_close(cur_proc(), fd);
}

// uint64_t sys_chdir(void) {
//   uint64_t path;
//   argaddr(&path, 0);
//
//   if (!valid_user_arg(path)) {
//     log("invalid user arg addr: 0x%08x\n", addr);
//     return -1;
//   } else {
//     struct proc *p = cur_proc();
//     char buf[size+1];
//     transtr(addr, buf, size, true);
//     struct dirent *old_cwd = p->cwd;
//     p->cwd = filesys_link(p, buf, buf, old_cwd->flag);
//     filesys_unlink(p, old_cwd);
//     return 0;
//   }
// }
