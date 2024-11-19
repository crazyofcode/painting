#include <types.h>
#include <macro.h>
#include <stdio.h>
#include <fs.h>

void fs_init(void) {
  return;
}

int file_open(const char *path, int flags) {
  panic("todo");
}
void file_close(int fd) {

  panic("todo");
}
int file_create(const char *path, mode_t mode) {

  panic("todo");
}
size_t file_read(int fd, uint64_t dst, size_t sz) {

  panic("todo");
}
size_t file_write(int fd, uint64_t src, size_t sz) {

  panic("todo");
}
void file_seek(int fd, off_t off, int flag) {

  panic("todo");
}
