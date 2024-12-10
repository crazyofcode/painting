#include <types.h>
#include <param.h>
#include <macro.h>
#include <stdio.h>
#include <list.h>
#include <fat32.h>
#include <fatfs.h>
#include <string.h>
#include <file.h>
#include <fs.h>

#define   MAX_FS_NUM    10
static struct filesystem fs[MAX_FS_NUM];

static bool is_equal_dirent(struct dirent *entry, const char *arg) {
  if (strncmp(entry->name, arg, strlen(arg)) == 0) {
    return true;
  } else {
    return false;
  }
}

void init_fs(void) {
  for (int i = 0; i < MAX_FS_NUM; i++) {
    fs[i].valid = false;
  }
}
struct filesystem *alloc_fs() {
  for (int i = 0; i < MAX_FS_NUM; i++) {
    if (fs[i].valid == false) {
      memset((void *)&fs[i], 0, sizeof(struct filesystem));
      fs[i].valid = true;
      return &fs[i];
    }
  }
  panic("no more fs to alloc");
}

static char *skip_slash(char *path) {
  while (*path == '/')
    ++path;
  return path;
}

static int walkDir(struct filesystem *fs, char *path, struct dirent *base_dir, struct dirent **dir,
                   struct dirent **file, char *lastElem, struct long_entry_set *longSet) {
  struct dirent *tdir, *tfile;
  char *p;

  if (path[0] == '/' || base_dir == NULL)
    tdir = fs->root;
  else
    tdir= base_dir;
  tfile = tdir;
  path = skip_slash(path);
  p = path;
  *file = NULL;

  while(*path != '\0') {
    char name[MAX_FILE_NAME_LEN];
    path = p;

    while(*p != '/' && *p != '\0')
      ++p;

    size_t name_len = p - path;
    if (name_len > MAX_FILE_NAME_LEN)
      return -E_BAD_PATH;
    strncpy(name, path, name_len);

    p = skip_slash(p);

    if (tfile->type == DIR_FILE) {
      // panic("dir not implemented");
      if (*p == '\0' && strncmp(name, tfile->name, name_len) == 0) {
        *file = tfile;
        return 0;
      } else
        return -E_NOT_FOUND;
    }
    if (strncmp(name, ".", 1) == 0) {
      continue;
    } else if (strncmp(name, "..", 2) == 0) {
      tfile = tfile->parent;
      continue;
    } else {
      tfile = list_find(&tfile->child, struct dirent, is_equal_dirent, name);
      if (tfile == NULL)
        return -E_NOT_FOUND;
      else {
        if (*p == '\0') {
          *dir = tfile;
          strncpy(lastElem, name, MAX_FILE_NAME_LEN);
          return 0;
        } else {
          tdir = tfile;
          continue;
        }
      }
    }
  }
  return 0;
}
bool createItemAt(struct dirent *base_dir, const char *path, struct dirent **file, mode_t mode, bool is_dir) {
  struct filesystem *fs;
	char lastElem[MAX_FILE_NAME_LEN];
	struct dirent *dir, *f;
	int r;
	struct long_entry_set longSet;

  if (base_dir) {
    fs = base_dir->filesystem;
  } else {
    fs = get_root_fs();
  }

  char cpath[strlen(path)+1];
  strncpy(cpath, path, strlen(path));
  if ((r = walkDir(fs, cpath, base_dir, &dir, &f, lastElem, &longSet)) == 0) {
    log("file already exist\n");
    return false;
  }
  panic("createItemAt todo");
  return false;
}

// static struct dirent *lookup_dirent(const char *path) {
  // struct proc *p = cur_proc();
  // struct dirent *base = NULL;
  //
  // if (path[0] == '/')
//   panic("lookup_dirent");
// }

struct dirent *get_file(struct dirent *base_dir, const char *path) {
  struct dirent *dirent;
  struct filesystem *fs;
  struct long_entry_set longSet;

  fs = base_dir == NULL ? get_root_fs() : base_dir->filesystem;

  int r = walkDir(fs, (char *)path, base_dir, 0, &dirent, 0, &longSet);
  if (r < 0)
    return NULL;
  else
    return dirent;
}

static uint32_t file_get_clusNo(struct dirent *file, uint32_t index) {
  uint32_t clus = file->first_cluster;
  for (int i = 0; i < index; i++) {
    clus = fatread(file->filesystem, clus);
    if (FAT32_isEOF(clus))
      break;
  }
  return clus;
}

/**
  * file: 需要读取的文件
  * dst:  从文件中读取的数据的目的地址
  * off:  文件的偏移量
  * n:    需要读取的字节数
  **/
int fileread(struct dirent *file, uint64_t dst, uint32_t off, uint32_t n) {
  if (off > file->size) {
    return -E_EXCEED_FILE;
  } else if ((off + n) > file->size) {
    n = file->size - off;
  }

  if (n == 0)   return 0;

  uint64_t start = off;
  uint64_t end   = off + n - 1;
  uint32_t clus_size = CLUSTER_SIZE(file->filesystem);
  uint32_t offset = off % clus_size;
  uint32_t len = 0;
  uint32_t clus = file_get_clusNo(file, start / CLUSTER_SIZE(fs));
  if (FAT32_isEOF(clus))
    return -1;

  clusread(file->filesystem, clus, offset, dst, MIN(n, clus_size - offset));
  len += MIN(n, clus_size - offset);

  clus = fatread(file->filesystem, clus);
  uint32_t clus_num = (end - start) / clus_size;
  for (int i = 0; i < clus_num; i++) {
    clusread(file->filesystem, clus, 0, dst + len, MIN(n - len, clus_size));
    clus = fatread(file->filesystem, clus);
    len += MIN(n - len, clus_size);
  }

  return len;
}

/**
  * file: 需要扩展的文件
  * new_size: 新的文件大小
  **/
static void fileExtend(struct dirent *file, uint32_t new_size) {
  if (file->size > new_size)  return;

  uint32_t clus_size = CLUSTER_SIZE(file->filesystem);
  uint32_t extend_clus = (new_size - file->size) / clus_size +
                          ((new_size - file->size) % clus_size == 0 ? 0 : 1);
  file->size = new_size;
  uint32_t clus = file->first_cluster;
  uint32_t last_clus = 0;
  for (; !FAT32_isEOF(clus);) {
    last_clus = clus;
    clus = fatread(file->filesystem, clus);
  }

  for (int i = 0; i < extend_clus; i++) {
    last_clus = clusalloc(file->filesystem, last_clus);
  }
}

int filewrite(struct dirent *file, uint64_t src, uint32_t off, uint32_t n) {
  if (n == 0)   return 0;

  if (off > file->size) {
    return -E_EXCEED_FILE;
  } else if (off + n > file->size) {
    fileExtend(file, off + n);
  }

  uint64_t start = off;
  uint64_t end   = off + n - 1;
  uint32_t clus_num = (end - start) % CLUSTER_SIZE(fs);
  uint32_t offset = off % CLUSTER_SIZE(fs);
  uint64_t clus = file_get_clusNo(file, start / CLUSTER_SIZE(fs));
  if (FAT32_isEOF(clus))
    return -1;
  uint32_t len = 0;

  cluswrite(file->filesystem, clus, offset, src, MIN(n, CLUSTER_SIZE(fs) - offset));
  len += MIN(n, CLUSTER_SIZE(fs) - offset);
  clus = fatread(file->filesystem, clus);

  for (int i = 0; i < clus_num; i++) {
    cluswrite(file->filesystem, clus, offset, src, MIN(n - len, CLUSTER_SIZE(fs)));
    len += MIN(n - len, CLUSTER_SIZE(fs));
    clus = fatread(file->filesystem, clus);
  }

  return len;
}
