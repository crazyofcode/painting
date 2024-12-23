#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <list.h>
#include <proc.h>
#include <macro.h>
#include <defs.h>
#include <vm.h>
#include <syscall.h>
#include <string.h>

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

uint64_t sys_getpid(void) {
  return cur_proc()->pid;
}

uint64_t sys_exec(void) {
  uint64_t ufile, uargv;
  argaddr(&ufile, 0);
  argaddr(&uargv, 1);

  char path[MAXLEN];
  char argv[MAXARG][MAXLEN];

  transtr(ufile, path, MAXLEN, true);
  transtr(uargv, argv[0], MAXLEN, true);
  memset(argv[1], 0, MAXLEN);

  if (process_execute(path, (const char **)argv))
    return 0;
  else
    return -1;
}

uint64_t sys_exit(void) {
  uint64_t state;
  argint(&state, 0);

  process_exit(state);
}
