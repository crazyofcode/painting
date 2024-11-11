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

static uint8_t *cluster_bit_map;
static void cluszero(struct filesystem *, uint32_t);
// init
// fill in superblock
int clusinit(struct filesystem *fs) {
  // 从设备中读取设备信息
  struct buf *buf = bread(fs->deviceNum, 0);
  if (buf == NULL)
    return ERR;

  struct fat32hder *header = (struct fat32hder *)(buf->data);
  memcpy(&fs->superblock, header, sizeof(*header));

  ASSERT_INFO(!strncmp((const char *)(header->system_identifier_string), "FAT32", 5), "read fat32 volume error");
  ASSERT_INFO(!strncmp((const char *)(fs->superblock.system_identifier_string), "FAT32", 5), "read fat32hdr error");

  // 记录 superblock 的信息
  // RootDirsectors = 0
  fs->sbinfo.first_data_sector = header->reserved_sectors + 
                                    header->number_fats * header->fat_size;
  fs->sbinfo.data_sector_cnt = header->large_sector_count - 
                                  fs->sbinfo.first_data_sector;
  fs->sbinfo.cluster_cnt = fs->sbinfo.data_sector_cnt /
                            header->sectors_per_cluster;
  fs->sbinfo.bytes_per_clus = header->bytes_per_sector *
                            header->sectors_per_cluster;

  // cluster bitmap info
  uint32_t len = fs->sbinfo.data_sector_cnt / 8 / 32;
  cluster_bit_map = kalloc(sizeof(uint8_t) * len, DEFAULT);
  memset(cluster_bit_map, 0, sizeof(uint8_t) * len);

  if (BSIZE != header->bytes_per_sector)
    return ERR;
  brelse(buf);

  strncpy(fs->root.name, "/", 1);
  fs->root.parent = NULL;
  fs->root.filesystem = fs;
  list_init(&fs->root.child);
  return 0;
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
  ASSERT_INFO(n < fs->sbinfo.bytes_per_clus - offset,
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
