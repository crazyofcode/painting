#include <types.h>
#include <param.h>
#include <spinlock.h>
#include <sleeplock.h>
#include <macro.h>
#include <fat32.h>
#include <list.h>
#include <fatfs.h>
#include <buf.h>
#include <string.h>
#include <defs.h>
#include <dirent.h>
#include <pm.h>
#include <riscv.h>
#include <stdio.h>

#define   FATDIR_PERM(entry) (*(char *)entry + SHORT_NAME_LEN)
#define   cal_cluster_pos(FstClusHI, FstClusLO) ((uint32_t)FstClusHI * (1 << 16) + (uint32_t)FstClusLO)
#define   is_long_file_name(entry)   ((entry->file_attribute & (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID))\
                                        == (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID))

static uint8_t *cluster_bit_map;
static void cluszero(struct filesystem *, uint32_t);
static void build_dirent_tree(struct dirent *);
// init
// fill in superblock
static int clusinit(struct filesystem *fs) {
  // 从设备中读取设备信息
  struct buf *buf = bread(fs->deviceNum, 0);
  if (buf == NULL)
    return ERR;

  struct fat32hder *header = (struct fat32hder *)(buf->data);
  fs->superblock.bytes_per_sector = header->bytes_per_sector;
  fs->superblock.sectors_per_cluster = header->sectors_per_cluster;
  fs->superblock.reserved_sectors = header->reserved_sectors;
  fs->superblock.cluster_root_directory = header->cluster_root_directory;
  fs->superblock.fat_size = header->fat_size;

  ASSERT_INFO(!strncmp((const char *)(header->system_identifier_string), "FAT32", 5), "read fat32 volume error");

  // 记录 superblock 的信息
  // RootDirsectors = 0
  fs->sbinfo.first_data_sector = header->reserved_sectors + 
                                    header->number_fats * header->fat_size;
  fs->sbinfo.data_sector_cnt = header->large_sector_count - 
                                  fs->sbinfo.first_data_sector;
  fs->sbinfo.cluster_cnt = fs->sbinfo.data_sector_cnt *
                            header->sectors_per_cluster;
  fs->sbinfo.bytes_per_clus = header->bytes_per_sector *
                            header->sectors_per_cluster;

  // cluster bitmap info
  uint32_t len = fs->sbinfo.data_sector_cnt / 8;
  cluster_bit_map = kalloc(sizeof(uint8_t) * len, DEFAULT);
  memset(cluster_bit_map, 0, sizeof(uint8_t) * len);

  if (BSIZE != header->bytes_per_sector)
    return ERR;
  brelse(buf);

  return 0;
}

// 读取簇内容
static void read_multiple_cluster(struct filesystem *fs, uint64_t clusNo, char *buf, uint32_t size) {
    for (uint32_t offset = 0; offset < size; offset += BSIZE) {
        clusread(fs, clusNo, offset, (uint64_t)(buf + offset), BSIZE);
    }
}

// 判断目录项是否无效
static int is_invalid_entry(struct FAT32Directory *entry) {
    return (entry->dir_name[0] == 0 || entry->dir_name[0] == FATDIR_DLT ||
            strncmp((const char *)entry->dir_name, ".          ", SHORT_NAME_LEN) == 0 ||
            strncmp((const char *)entry->dir_name, "..         ", SHORT_NAME_LEN) == 0);
}

// 处理长文件名
static int handle_long_file_name(char *buf, uint32_t offset, int longEntSeq) {
    struct FAT32LongDirectory *longEnt = (struct FAT32LongDirectory *)(buf + offset);
    int prevSeq = longEntSeq;
    longEntSeq = FATLDIR_SEQ(*(char *)longEnt);

    char *rawName = buf + 512 + longEntSeq * 13;
    memset(rawName, 0, 13);
    if (prevSeq == 0) rawName[13] = '\0';
    wstr2str(rawName, (char *)longEnt + 1, 10);
    wstr2str(rawName + 5, (char *)longEnt + 14, 12);
    wstr2str(rawName + 11, (char *)longEnt + 28, 4);

    return longEntSeq;
}

// 填写文件名
static void fill_file_name(char *name, struct FAT32Directory *entry) {
    if (is_long_file_name(entry)) {
        fill_fat32_long_name(name, (char *)entry);
    } else {
        read_fat32_short_name(name, (char *)entry->dir_name);
    }
}

// 创建目录项
struct dirent *create_dirent_entry(struct dirent *parent, struct FAT32Directory *entry, uint32_t offset, struct dirent **lastDir) {
    struct dirent *newDir = dirent_alloc();
    ASSERT_INFO(newDir, "dirent run out!");

    newDir->filesystem = parent->filesystem;
    newDir->first_cluster = cal_cluster_pos(entry->dir_FstClusHI, entry->dir_FstClusLO);
    newDir->size = entry->dir_FileSize;
    newDir->mode |= ((FATDIR_PERM(entry) & ATTR_DIRECTORY) ? ATTR_DIRECTORY : ATTR_FILE);
    newDir->parent = parent;
    newDir->linkcnt = 1;
    newDir->parent->offset = offset;

    list_push_back(&parent->child, &newDir->elem);
    if (newDir->mode & ATTR_DIRECTORY)
      list_init(&newDir->child);

    fill_file_name(newDir->name, entry);

    log("%s\n", newDir->name);
    return newDir;
}

// 解析簇中的目录项
static int parse_directory_entries(struct dirent *parent, char *buf, uint32_t clusSize, struct dirent **lastDir) {
    bool longEntSeq = false;

    for (uint32_t i = 0; i < clusSize; i += DIRECTORY_SIZE) {
        struct FAT32Directory *entry = (struct FAT32Directory *)(buf + i);

        if (is_invalid_entry(entry)) {
            longEntSeq = false;
            continue;
        }

        if (is_long_file_name(entry)) {
            longEntSeq = handle_long_file_name(buf, i, longEntSeq);
            continue;
        }

        struct dirent *newDir = create_dirent_entry(parent, entry, i, lastDir);

        if ((newDir->mode & ATTR_DIRECTORY) == ATTR_DIRECTORY) {
            build_dirent_tree(newDir);  // 递归构建子目录
        }
    }
    return 0;
}

// 构建目录树
static void build_dirent_tree(struct dirent *dir) {
    uint64_t clusNo = dir->first_cluster;
    struct filesystem *fs = dir->filesystem;
    uint32_t clusSize = fs->sbinfo.bytes_per_clus;
    char *buf = kpmalloc();
    struct dirent *lastDir = NULL;

    while (1) {
        read_multiple_cluster(fs, clusNo, buf, clusSize);
        int result = parse_directory_entries(dir, buf, clusSize, &lastDir);

        if (result == -1) {
            break;  // 到达文件尾或无效簇
        }

        clusNo = fatread(fs, clusNo);  // 获取下一个簇号
        if (FAT32_isEOF(clusNo)) {
            break;  // 到达文件尾
        }
    }
    kpmfree(buf);
}

/**
 * @brief 计数文件的簇数
 */
static int count_clusters(struct dirent *file) {
	log("count Cluster begin!\n");

	int clus = file->first_cluster;
	int i = 0;
	if (clus == 0) {
		log("cluster is 0!\n");
		return 0;
	}
	// 如果文件不包含任何块，则直接返回0即可。
	else {
		while (!FAT32_isEOF(clus)) {
			clus = fatread(file->filesystem, clus);
			i += 1;
		}
		log("count Cluster end!\n");
		return i;
	}
}

void fat32_init(struct filesystem *fs) {
  log("init fat32 ...\n");
  strncpy(fs->name, "FAT32", 5);

  ASSERT_INFO(clusinit(fs) == 0, "clusinit fault");
  log("clus init finish\n");

  fs->root = dirent_alloc();
  fs->root->first_cluster = fs->superblock.cluster_root_directory;
  strncpy(fs->root->name, "/", 1);
  fs->root->filesystem = fs;
  fs->root->mode |= ATTR_DIRECTORY;
  fs->root->parent = NULL;
  fs->root->linkcnt = 1;
  fs->root->offset = 0;
  fs->root->size = count_clusters(fs->root) * CLUSTER_SIZE(fs);
  list_init(&fs->root->child);
  ASSERT_INFO(sizeof(struct FAT32Directory) == DIRENT_SIZE, "FAT32Directory size is not DIRENT_SIZE");

  build_dirent_tree(fs->root);
  log("fat fs init finish\n");
}

uint32_t fatread(struct filesystem *fs, uint32_t clusterNo) {
  if (clusterNo < fs->superblock.cluster_root_directory ||
      clusterNo > fs->sbinfo.data_sector_cnt + 1)
    return 0;

  // zero -> current fat
  uint32_t sectorNo   = SectorNum(fs->superblock, clusterNo, 0);
  uint32_t sectorOff  = FATEntOffset(fs->superblock, clusterNo);

  struct buf *b = bread(fs->deviceNum, sectorNo);
  uint32_t *data = (uint32_t *)(b->data);
  uint32_t ret = data[sectorOff / 4];
  brelse(b);
  return ret;
}

void fatwrite(struct filesystem *fs, uint32_t clusterNo, uint32_t nextNo) {
  if (clusterNo < fs->superblock.cluster_root_directory ||
      clusterNo > fs->sbinfo.data_sector_cnt + 1) {
    ASSERT_INFO(0, "write fat");
    return;
  }

  uint32_t sectorNo0  =  SectorNum(fs->superblock, clusterNo, 0);
  uint32_t sectorNo1  =  SectorNum(fs->superblock, clusterNo, 1);
  uint32_t sectorOff  =  FATEntOffset(fs->superblock, clusterNo);

  struct buf *b = bread(fs->deviceNum, sectorNo0);
  uint32_t *data = (uint32_t *)(b->data);
  data[sectorOff / 4] = nextNo;
  bwrite(b);

  struct buf *b1 = bread(fs->deviceNum, sectorNo1);
  data = (uint32_t *)(b->data);
  data[sectorOff / 4] = nextNo;
  bwrite(b1);

  brelse(b);
  brelse(b1);
}

uint32_t clusalloc(struct filesystem *fs, uint32_t prevNo) {
  uint32_t len = fs->sbinfo.data_sector_cnt >> 8;
  for (int i = 0; i < len; i++) {
    if (cluster_bit_map[i] != 0xff) {
      uint32_t nextNo = (i << 8) +
        find_lowest_zero_bit(cluster_bit_map[i]);
      cluster_bit_map[i] |= MASK(cluster_bit_map[i]);
      if (prevNo != 0) {
        fatwrite(fs, prevNo, nextNo);
      }
      fatwrite(fs, nextNo, FAT32_EOF);
      cluszero(fs, nextNo);
      return nextNo + 2;
    }
  }
  log("out of dat32 disk volume");
  return 0;
}

void clusfree(struct filesystem *fs, uint32_t clusNo, uint32_t prevNo) {
  if (prevNo != 0) {
    fatwrite(fs, prevNo, FAT32_EOF);
  }
  fatwrite(fs, clusNo, 0);
  uint32_t idx = (clusNo-2) >> 8;
  uint32_t off = (clusNo-2) & (BIT_OFF);
  cluster_bit_map[idx] &= (~(1 << off));
}

static void cluszero(struct filesystem *fs, uint32_t clusNo) {
  uint32_t sectorSize = fs->superblock.bytes_per_sector;
  uint32_t clusSector = fs->superblock.sectors_per_cluster;
  uint32_t idx = first_sector_clus(fs, clusNo);
  for (uint32_t i = 0; i < clusSector; idx++, i++) {
    struct buf *b = bread(fs->deviceNum, idx);
    memset(b->data, 0, sectorSize);
    bwrite(b);
    brelse(b);
  }
}

uint32_t clusread(struct filesystem *fs, uint32_t clusNo, uint32_t offset
                  ,uint64_t dst, uint64_t n) {
  // 找到需要读的第一个 sector
  uint32_t secNo = first_sector_clus(fs, clusNo) + offset /
                        fs->superblock.bytes_per_sector;
  uint32_t secOff = offset % fs->superblock.bytes_per_sector;
  ASSERT_INFO(n <= fs->sbinfo.bytes_per_clus - offset,
              "clusread: The size of the \
              required read exceeds the remaining space in the cluster.");
  uint32_t read_len = 0;
  uint32_t len = MIN(fs->superblock.bytes_per_sector - secOff, n);
  struct buf *b = bread(fs->deviceNum, secNo);
  memcpy((void *)dst + read_len, (void *)&b->data[secOff], len);
  brelse(b);
  if (len == n) return len;
  read_len += len;
  for (; read_len < n; secNo++) {
    len = MIN(fs->superblock.bytes_per_sector, n - read_len);
    b = bread(fs->deviceNum, secNo);
    memcpy((void *)dst + read_len, b->data, len);
    brelse(b);
    read_len += len;
  }
  return read_len;
}

uint32_t cluswrite(struct filesystem *fs, uint32_t clusNo, uint32_t offset
                   ,uint64_t src, uint64_t n) {
  uint32_t secNo = first_sector_clus(fs, clusNo) + offset /
                        fs->superblock.bytes_per_sector;
  uint32_t secOff = offset % fs->superblock.bytes_per_sector;
  ASSERT_INFO(n < fs->sbinfo.bytes_per_clus - offset,
              "cluswrite: The size of the \
              required write exceeds the remaining space in the cluster.");

  uint32_t write_len = 0;
  uint32_t len = MIN(fs->superblock.bytes_per_sector - secOff, n);
  struct buf *b = bread(fs->deviceNum, secNo);
  memcpy((void *)&b->data[secOff], (void *)src + write_len, len);
  bwrite(b);
  brelse(b);
  if (len == n) return len;
  write_len += len;
  for (; write_len < n; secNo++) {
    len = MIN(fs->superblock.bytes_per_sector, n - write_len);
    b = bread(fs->deviceNum, secNo);
    memcpy(b->data, (void *)src + write_len, len);
    bwrite(b);
    brelse(b);
    write_len += len;
  }
  return write_len;
}
