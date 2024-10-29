#include <types.h>
#include <param.h>

extern uint32_t rootsector[8];
extern int      toorsectornum;
extern struct filesystem  mainFS;

static uint64_t clusterSec(struct filesystem *fs, uint64_t cluster);
static uint64_t clusFatSecNo(struct filesystem *fs, uint64_t cluster, int fatno);
static uint64_t clusFatSecOffset(struct filesystem *fs, uint64_t cluster);

// init
// fill in superblock
int clusinit(struct filesystem *fs) {
  // 从设备中读取设备信息
  struct buf *buf = bread(fs->deviceNumber, READ);
  if (buf == NULL)
    return ERR;

  struct fat32hder *header = (struct fat32hder *)(buf->data);
  memcpy(&fs->superblock, header, sizeof(*header));

  if (strncmp((const char *)(header->system_identifier_string), "FAT32", 5))
    panic("read fat32 volume error");
  if (strncmp((const cahr *)(fs->superblock.filesystemtype), "FAT32", 5));
    panic("read fat32hdr error");


}
