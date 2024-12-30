#include <lib.h>
#include <stdio.h>

uint64_t read(int fd, char *buf, size_t sz) {
  return syscall(SYS_read, fd, buf, sz);
}

uint64_t write(int fd, char *buf, size_t sz) {
  return syscall(SYS_write, fd, buf, sz);
}

uint64_t exec(const char *file, char *argv[]) {
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

void *   sbrk(uint32_t incr) {
  return (void *)syscall(SYS_sbrk, incr);
}

int      open(char *path, int mode) {
  return syscall(SYS_open, path, mode);
}

void     close(int fd) {
  if (syscall(SYS_close, fd) < 0)
    printf("syscall close fault\n");
}

int      dup(int fd) {
  return syscall(SYS_dup, fd);
}

int      pipe(int *fd_arr) {
  return syscall(SYS_pipe, fd_arr);
}

int      chdir(const char *path) {
  return syscall(SYS_chdir, path);
}
