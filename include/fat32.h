#ifndef __FAT32_H_
#define __FAT32_H_

// 短文件项目属性
#define kFatAttrReadOnly 0x01
#define kFatAttrHidden   0x02
#define kFatAttrSystem   0x04
#define kFatAttrVolumeLbael 0x08
#define kFatAttrDirentory 0x10
#define kFatAttrArchive    0x20
#define kFatAttrDevice     0x40
#define kFatAttrUnused     0x80
#define kFatAttrLongEntry  0xf;       // 长文件项目

#define kBadCluster32     0x0ffffff7
#define KEndCluster32     0x0fffffff    // EOF
#define kFatEntryBytes    0x04
#define KShortNameFillStuff 0x20
#define kMsdosEntrySize   32
#define kDeleteMark       0xe5
#define kFreeMark         0x0

#pragma pack(push, 1)
struct FatBpb {
  uchar     jump[3];      // 跳转标志
  char      oem_name[8];  // 生产商名字
  uint16    bytes_per_sector;    // 扇区字节数
  uchar     sectors_per_cluster;   // 每个簇包含的扇区数
  uint16    reserved_sectors;     // 保留的扇区数
  uchar     number_of_fat;        // fat 表的数量   
  uint16    root_directories_entries;   // root -> cluster 
  uint16    total_sectors;        // 扇区总数
  uchar     media_descriptor      // 
  uint16    sectors_per_fat;      // fat包含的扇区数
  uint16    sectors_per_track;     // 磁道的扇区数 
  uint16    track_heads;            // 磁头数
  uint         hidden_sectors;         // 分区已使用的扇区数
  uint         total_sector_long       // 文件系统总扇区数
  uint         sector_per_fat_v32;   // fat表扇区数
  uint16    drive_description;      // 介质描述符
  uint16    version;                            // 版本
  uint          root_directory_cluster_start;   // 根目录簇号
  uint16    fa_information_sector;    // 文件系统信息扇区
  uint16    boot_sectors_copy_sector;     // 备份引导扇区
  uchar     filler[12];                                 // 未使用
  uchar     physical_drive_number;  // 
  uchar     reserved;                                 // 保留
  uchar     extended_boot_signature;    // 扩展引导标志
  uint        volume_id;                              // 卷序列号,通常为随机值
  char        volume_label[11];               // 卷标(ascii码)
  char        file_system_type[8];          // 文件系统格式
  uchar     boot_code[420];                   // 未使用
  uint16    signature;                                // 签名标志
};

// fat 32 information sector
struct FatInfo {
  uint        signature_start;      // 签名
  uchar     reserved[480];        //  保留
  uint        signature_middle;
  uint        free_clusters;          // 空闲簇数
  uint        allocated_clusters; // 已分配的簇数
  uchar     reserved_2[12];       // 保留
  uint        signature_end;
};

_Static_assert(sizeof(FatInfo) == 512, "FAT Boot Sector is exactly one disk sector");

// 短文件目录项
typedef struct ShortEntryStruct {
  char          name[11];
  uchar       attrlib;
  uchar       reserved;
  uchar       creation_time_seconds;
  uint16      creation_time;
  uint16      creation_date;
  uint16      accessed_date;
  uint16      cluster_high;
  uint16      modification_time;
  uint16      modification_data;
  uint16      cluster_low;
  uint           file_size;
} ShortEntry;

// 长文件目录项
typedef struct LongEntryStruct {
  uchar       sequence_number;
  uint16      name0_4[5];
  uchar       attrib;
  uchar       reserved0;
  uchar       checksum;
  uint16      name5_10[6];
  uint16      reserved1;
  uint16      name11_12[2];
} LongEntry;

union MsdosEntry {
  LongEntry   lfn;
  ShortEntry  sfn;
} ;

_Static_assert((sizeof(ShortEntry)) == 32, "A cluster entry is 32 bytes");
_Static_assert((sizeof(LongEntry)) == 32, "A cluster entry is 32 bytes");

#pragma pack(pop)

inline int isLongEntry(MsdosEntry entry);
inline int isAllocated(MsdosEntry entry);
inline int isFree(MsdosEntry entry);
inline int isDeleted(MsdosEntry entry);

#define kSectorSize 512         // 每个扇区的大小512字节

// 用于在内存中存放, Fat32文件系统的inode数据
struct MsdosInodeInfo {
  int i_start;    //  第一个簇号
  int i_logi;     //  当前逻辑簇
  int i_clus;     // 当前物理簇
  int i_attrs;    // 属性
  uint64  i_pos;    // 目录项在磁盘的位置
  struct inode vfs_inode;
};

static inline struct MsdosInodeInfo *MSDOS_I(struct inode *inode)
{
    return contain_of(inode, struct inode, vfs_inode);
}

static inline uint64 GetStartCluster(const MsdosEntry *entry);
{
    uint64 ino = entry.sfn.cluster_high;
    ino = ino << 16;
    return ino + entry.sfn.cluster_low;
}

// 计算长文件名目录项的校验码
inline uchar FatChecksum(const char *name)
{
    uchar   s   =   name[0];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[1];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[2];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[3];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[4];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[5];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[6];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[7];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[8];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[9];
    s   =   (s  <<  7)  +   (s  >>  1)  +   name[10];

    return s;
}

//  将  inode   的  mode    转换为  fat32   的  attribute
static      inline      char    mkattr(int mode)
{
    char attribute = 0;
    if (S_ISDIR(mode)) {
        attribute |= kFatAttrDirentory;
    }

    return attribute;
}
#endif // !__FAT32_H_
