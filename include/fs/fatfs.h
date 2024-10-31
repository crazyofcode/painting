#ifndef FATFS_H__
#define FATFS_H__
#define   MAXLEN      64

// 需要在创建时初始化 list
struct direntlist {
  struct dirent *dirent;
  struct list_elem  elem;
};

struct dirent {
  char    name[MAXLEN];
  uint64_t  size;
  enum {DIR_DIR, DIR_FILE, DIR_LINK} type;    // 目录, 文件, 链接
  struct filesystem *filesystem;  // 所属的文件系统
  uint32_t  linkcnt;              // 链接数量
  struct dirent *parent;
  struct direntlist child;
}

struct filesystem {
  bool                      valid;
  char                      name[7];
  struct fat32hder          superblock;
  struct SubSuperBlockInfo  sbinfo;
  struct dirent             root;   // 根目录
  struct dirent *           image;
  struct dirent *           mountPoint;
  /*struct FSInfo             fsinfo;*/
  int                       deviceNum;
};

// fat32.c
int           clusinit(struct filesystem *);
uint32_t      fatread(struct filesystem *, uint32_t);
uint32_t      fatwrite(struct filesystem *, uint32_t, uint32_t);
uint32_t      fatalloc(struct filesystem *, uint32_t);
void          fatfree(struct filesystem *, uint32_t, uint32_t);
uint32_t      clusread(struct filesystem *, uint32_t, uint32_t, uint64_t, uint64_t);
uint32_t      cluswrite(struct filesystem *, uint32_t, uint32_t, uint64_t, uint64_t);
#endif // !FATFS_H__
