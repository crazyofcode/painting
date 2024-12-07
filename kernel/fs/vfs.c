#include <types.h>
#include <macro.h>
#include <file.h>
#include <dirent.h>
#include <vfs.h>

bool filesys_create(uint64_t path, mode_t mode) {
  struct dirent *file = dirent_alloc();
  ASSERT_INFO(file != NULL, "alloc dirent fault");
  if (!file_create(path, mode, &file)) {
    dirent_free(file);
    log("create file %s fault\n", path);
    return false;
  }
  return true;
}

int filesys_open(uint64_t path, int flags) {
  struct dirent *dirent = file_open(path, flags);
  if (dirent == NULL) {
    return -1;
  }
  struct file *file = kalloc(sizeof(struct file), FILE_MODE);
  file->flag = flag;
  file->dirent = dirent;
  file->fd = alloc_fd();
  file->pos = 0;
  file->deny_write = false;
  list_push_back(&cur_proc()->file_list, &file->elem);
  return file->fd;
}
