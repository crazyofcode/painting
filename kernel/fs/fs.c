#include <types.h>
#include <param.h>
#include <macro.h>
#include <stdio.h>
#include <list.h>
#include <fat32.h>
#include <fatfs.h>
#include <string.h>
#include <fs.h>

#define   MAX_FS_NUM    10
static struct filesystem fs[MAX_FS_NUM];

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

static int walkDir(struct filesystem *fs, const char *path, struct dirent *base_dir, struct dirent **dir,
                   struct dirent **f, char *lastElem, longEntSet *longSet) {
  struct dirent *tdir, tfile;
  char name[MAX_FILE_NAME_LEN];
  char *p;
  int r;

  if (path[0] == '/' || base_dir == NULL)
    tfile = &fs->root;
  else
    tfile = base_dir;
  memset(name, 0, MAX_FILE_NAME_LEN);
  path = skip_slash(path);
  p = path;
  *file = NULL;

  while(*path != '\0') {
    tdir = tfile;
    path = p;

    while(*p != "/" && *p != '\0')
      ++p;

    if (p - path > MAX_FILE_NAME_LEN)
      return -E_BAD_PATH;
    strncpy(name, path, p-path);

    p = skip_slash(p);

    if (tfile->type != DIR_DIR)
  }
}
bool createItemAt(struct dirent *base_dir, const char *path, struct dirent **file, mode_t mode, bool is_dir) {
  struct filesystem *fs;
	char lastElem[MAX_NAME_LEN];
	Dirent *dir, *f;
	int r;
	longEntSet longSet;

  if (base_dir) {
    fs = base_dir->fs;
  } else {
    fs = fatFs;
  }

  if ((r = walkDir(fs, path, base_dir, &dir, &f, lastElem, &longSet)) == 0) {
    log("file already exist\n");
    return false;
  }
  panic("createItemAt todo");
  return false;
}

struct dirent *lookup_dirent(const char *path) {
  panic("lookup_dirent");
}
