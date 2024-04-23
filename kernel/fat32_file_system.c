#include "types.h"
#include "param.h"
#include "fat32_file_system.h"
#include "fat32.h"
#include "fs.h"
#include "defs.h"

// fat32 文件系统维护的内容
Fat32Info   info_;

static FatBpb fat_bpb_;   // fat32 启动扇区
static FatFsInfo  fat_fs_info_;   // fat32 信息扇区
int           dev;    // 挂载设备号
int           max_inode_num;      // inode 缓存最大数量
uint64        inode_cache_inode_key[max_inode_num];
struct        inode_cache_inode_value[max_inode_num];

int
fat32init()
{
  memset((char *)fat_fs_info_, 0, 512);
  memset((char *)fat_bpb_, 0, 512);

}
