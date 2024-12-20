#include "lib.h"

uint64_t write(int fd, char *buf, size_t sz) {
  return syscall(SYS_write, fd, sz);
}
