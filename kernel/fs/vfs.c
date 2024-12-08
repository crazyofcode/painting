#include <types.h>
#include <macro.h>
#include <file.h>
#include <dirent.h>
#include <vfs.h>

static struct file *find_file(struct list *list, int fd) {
  struct list_elem *e;
  for (e = list_begin(list); e != list_end(list); e = list_next(e)) {
    struct file *file = list_entry(e, struct file, elem);
    if (file->fd == fd)
      return file;
  }
  return NULL;
}

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

off_t filesys_write(int fd, uint64_t addr, size_t size) {
  struct proc *p = cur_proc();
  struct file *file = find_file(&p->file_list, fd);
  if (file == NULL)
    return -1;
  size = file_write(file->dirent, addr, size);
  file->pos += size;
  return size;
}

off_t filesys_read(int fd, uint64_t buf, size_t size) {
  struct proc *p = cur_proc();
  struct file *file = find_file(&p->file_list, fd);
  if (file == NULL)
    return -1;
  size = file_read(file->dirent, buf, size);
  file->pos += size;
  return size;
}
