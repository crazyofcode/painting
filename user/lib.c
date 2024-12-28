#include <lib.h>

uint64_t write(int fd, char *buf, size_t sz) {
  return syscall(SYS_write, fd, buf, sz);
}

uint64_t exec(const char *file, const char *argv[]) {
  return syscall(SYS_exec, file, argv);
}

void     exit(int state) {
  syscall(SYS_exit, state);  // 使用系统调用退出程序
}

int      fork(void) {
  return syscall(SYS_fork);
}

int      wait(int pid) {
  return syscall(SYS_wait, pid);
}
