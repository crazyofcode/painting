#include <types.h>
#include <macro.h>
#include <stdio.h>
#include <list.h>
#include <fat32.h>
#include <fatfs.h>
#include <file.h>

struct filesystem *fat_fs;

void filesys_init(void) {
  fat_fs = alloc_fs();

  fat_fs->image = NULL;
  fat_fs->deviceNum = 0;

  fat32_init(fat_fs);
}

int file_open(const char *path, int flags) {
  panic("todo");
}
void file_close(int fd) {

  panic("todo");
}
bool file_create(const char *path, mode_t mode) {
  // struct dirent *dir = NULL;
  // struct proc *p = cur_proc();
  // char filename[MAX_FILE_NAME_LEN];
  //
  // dir = p->cwd;
  // copyin(p->pagetable, filename, (uint64_t)path, strlen(path));
  //
  // struct dirent *file;
  // return createItemAt(dir, filename, &file, mode, false);
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
