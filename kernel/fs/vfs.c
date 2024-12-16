#include <types.h>
#include <param.h>
#include <macro.h>
#include <list.h>
#include <spinlock.h>
#include <proc.h>
#include <defs.h>
#include <file.h>
#include <fat32.h>
#include <fatfs.h>
#include <dirent.h>
#include <string.h>
#include <fs.h>
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

bool filesys_create(struct proc *p, char *path, mode_t mode) {
  struct dirent *base = p->cwd;
  struct dirent *file = dirent_alloc();
  ASSERT_INFO(file != NULL, "alloc dirent fault");
  if (!file_create(base, path, mode, file)) {
    dirent_free(file);
    log("create file %s fault\n", path);
    return false;
  }
  return true;
}

int filesys_open(struct proc *p, char *path, int flags) {
  struct dirent *base = p->cwd;
  struct dirent *dirent = file_open(base, path, flags);
  if (dirent == NULL) {
    return -1;
  }
  struct file *file = kalloc(sizeof(struct file), FILE_MODE);
  file->flag = flags;
  file->dirent = dirent;
  file->fd = alloc_fd();
  file->pos = 0;
  file->deny_write = false;
  list_push_back(&p->file_list, &file->elem);
  return file->fd;
}

off_t filesys_write(struct proc *p, int fd, char *addr, size_t size) {
  struct file *file = find_file(&p->file_list, fd);
  if (file == NULL)
    return -1;
  size = file_write(file->dirent, addr, file->pos, size);
  file->pos += size;
  return size;
}

off_t filesys_read(struct proc *p, int fd, char *buf, size_t size) {
  struct file *file = find_file(&p->file_list, fd);
  if (file == NULL)
    return -1;
  size = file_read(file->dirent, buf, file->pos, size);
  file->pos += size;
  return size;
}

bool filesys_close(struct proc *p, int fd) {
  struct file *file = find_file(&p->file_list, fd);
  if (file == NULL)
    return true;
  else {
    struct dirent *dirent = file->dirent;
    list_remove(&file->elem);
    kfree(file, FILE_MODE);
    return file_close(dirent);
  }
}

static bool contian_of(struct file *file, char *name) {
  if (strncmp(file->dirent->name, name, strlen(name)) == 0)
    return true;
  else
    return false;
}
bool filesys_remove(struct proc *p, char *filename) {
  struct file *file = list_find(&p->file_list, struct file, contian_of, filename);
  bool ret = false;
  if (file != NULL) {
    struct dirent *dirent = file->dirent;
    list_remove(&file->elem);
    kfree(file, FILE_MODE);
    ret = file_remove(dirent, filename);
    if (!file_close(dirent))
      log("Failure to close file before removing it\n");
  }
  return ret;
}

void filesys_seek(struct proc *p, int fd, off_t offset, int mode) {
  struct file *file = find_file(&p->file_list, fd);
  switch (mode) {
    case SEEK_SET:
      file->pos = offset;
      break;
    case SEEK_CUR:
      file->pos += offset;
      break;
    case SEEK_END:
      file->pos = file->dirent->size;
      break;
    default:
      log("invalid mode\n");
      ASSERT(0);
  }
}
bool filesys_link(char *oldpath, char *newpath, int flags) {
  ASSERT(0);
  return true;
}
