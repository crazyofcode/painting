#ifndef FATFS_H__
#define FATFS_H__
#define   MAXLEN      64

struct dirent {
  char    name[MAXLEN];
  uint64_t  size;
  uint32_t  first_cluster;
  uint32_t  offset;
  /*enum {DIR_DIR, DIR_FILE, DIR_LINK} type;    // 目录, 文件, 链接*/
  struct filesystem *filesystem;  // 所属的文件系统
  uint32_t  linkcnt;              // 链接数量
  struct dirent *parent;
  struct list child;
  struct list_elem elem;
  mode_t      mode;
};

struct filesystem {
  bool                      valid;
  char                      name[7];
  struct superblock         superblock;
  struct SubSuperBlockInfo  sbinfo;
  struct dirent *           root;   // 根目录
  struct dirent *           image;
  struct dirent *           mountPoint;
  /*struct FSInfo             fsinfo;*/
  int                       deviceNum;
	struct buf *(*get)(struct filesystem *fs, u64 blockNum); // 读取FS的一个Buffer
};

// fat32.c
void          fat32_init(struct filesystem *);
void          fatinit(struct filesystem *);
/*int           clusinit(struct filesystem *);*/
uint32_t      fatread(struct filesystem *, uint32_t);
void          fatwrite(struct filesystem *, uint32_t, uint32_t);
uint32_t      fatalloc(struct filesystem *, uint32_t);
void          fatfree(struct filesystem *, uint32_t, uint32_t);
uint32_t      clusread(struct filesystem *, uint32_t, uint32_t, uint64_t, uint64_t);
uint32_t      cluswrite(struct filesystem *, uint32_t, uint32_t, uint64_t, uint64_t);
uint32_t      count_clus(struct dirent *);
#endif // !FATFS_H__
