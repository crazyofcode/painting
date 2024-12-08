#include <types.h>
#include <param.h>
#include <macro.h>
#include <stdio.h>
#include <list.h>
#include <spinlock.h>
#include <proc.h>
#include <fat32.h>
#include <fatfs.h>
#include <file.h>
#include <dirent.h>
#include <string.h>
#include <vm.h>
#include <defs.h>
#include <fs.h>

struct filesystem *fat_fs;
struct spinlock fd_lock;
unsigned char fd_bitmap[MAX_DIRENT>>8];
// free fd
void fd_free(int fd) {
  acquire(&fd_lock);
  if (fd >= 0 && fd < MAX_DIRENT) {
      fd_bitmap[fd >> 3] &= ~(1 << (fd & (~(0x07)))); // 释放该fd号，将该位设为0
  }
  release(&fd_lock);
}

// alloc fd
int fd_alloc(void) {
  acquire(&fd_lock);
  for (int i = 0; i < MAX_DIRENT; i++) {
      if ((fd_bitmap[i >> 8] & (1 << (i & (~(0x07))))) == 0) {
          fd_bitmap[i >> 8] |= (1 << (i & (~(0x07)))); // 设置该位为1，表示已分配
          release(&fd_lock);
          return i;
      }
  }
  release(&fd_lock);
  return -1; // 没有可用的fd
}

void filesys_init(void) {
  fat_fs = alloc_fs();
  dirent_init();

  fat_fs->image = NULL;
  fat_fs->deviceNum = 0;

  initlock(&fd_lock, "fd_lock");
  memset(fd_bitmap, 0, (sizeof(unsigned char) * (MAX_DIRENT >> 8)));
  fat32_init(fat_fs);
}

struct dirent *file_open(uint64_t path, int flags) {
  char filename[MAX_FILE_NAME_LEN];
  struct proc *p = cur_proc();
  copyin(p->pagetable, filename, path, MAX_FILE_NAME_LEN);
  struct dirent *dirent = lookup_dirent(filename);
  // check permission
  if ((~(dirent->mode)) & flags) {
    printf("permission deny\n");
    return NULL;
  }
  return dirent;
}
void file_close(int fd) {

  panic("todo2");
}
bool file_create(uint64_t path, mode_t mode, struct dirent *file) {
  struct proc *p = cur_proc();
  char filename[MAX_FILE_NAME_LEN];

  struct dirent *dir = p->cwd;
  copyin(p->pagetable, filename, path, MAX_FILE_NAME_LEN);

  return createItemAt(dir, filename, &file, mode, false);
}
size_t file_read(int fd, uint64_t dst, size_t sz) {

  panic("todo4");
}
size_t file_write(int fd, uint64_t src, size_t sz) {

  panic("todo5");
}
void file_seek(int fd, off_t off, int flag) {

  panic("todo6");
}
