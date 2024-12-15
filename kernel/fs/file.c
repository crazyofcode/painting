#include <types.h>
#include <param.h>
#include <macro.h>
#include <list.h>
#include <spinlock.h>
#include <proc.h>
#include <fat32.h>
#include <fatfs.h>
#include <file.h>
#include <dirent.h>
#include <string.h>
#include <fs.h>

struct filesystem *fat_fs;
struct spinlock fd_lock;
unsigned char fd_bitmap[MAX_DIRENT>>8];

struct filesystem *get_root_fs(void) {
  return fat_fs;
}
// free fd
void free_fd(int fd) {
  acquire(&fd_lock);
  if (fd >= 0 && fd < MAX_DIRENT) {
      fd_bitmap[fd >> 3] &= ~(1 << (fd & (~(0x07)))); // 释放该fd号，将该位设为0
  }
  release(&fd_lock);
}

// alloc fd
int alloc_fd(void) {
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

struct dirent *file_open(struct dirent *base, char *path, int flags) {
  struct dirent *dirent = get_file(base, path);
  if (dirent == NULL) {
    log("not found\n");
    return NULL;
  }
  // check permission
  if ((~(dirent->mode)) & flags) {
    log("permission deny\n");
    return NULL;
  }
  return dirent;
}

bool file_close(struct dirent *dirent) {
  if (dirent == NULL)
    return false;
  dirent_free(dirent);
  return true;
}
bool file_create(struct dirent *dir, char *path, mode_t mode, struct dirent *file) {
  return createItemAt(dir, path, &file, mode, false);
}
size_t file_read(struct dirent *dirent, char *dst, off_t offset, size_t sz) {
  if (dirent == NULL || dst == NULL)
    return -1;
  if (sz == 0)
    return 0;
  return fileread(dirent, (uint64_t)dst, offset, sz);
}
size_t file_write(struct dirent *dirent, char *src, off_t offset, size_t sz) {
  if (dirent == NULL || src == NULL)
    return -1;
  if (sz == 0)
    return 0;
  return filewrite(dirent, (uint64_t)src, offset, sz);
}

bool file_remove(struct dirent *base, char *filename) {
  struct dirent *dirent = get_file(base, filename);
  uint32_t clus = dirent->first_cluster;

  uint32_t clus_size = CLUSTER_SIZE(dirent->filesystem);
  uint32_t clus_num = dirent->size / clus_size + (dirent->size % clus_size == 0 ? 0 : 1);

  for(int i = 0; i < clus_num; i++) {
    uint32_t tmp = clus;
    if (FAT32_isEOF(tmp))
      return false;
    clus = fatread(dirent->filesystem, clus);
    clusfree(dirent->filesystem, tmp, 0);
  }
  return true;
}

