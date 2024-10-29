#ifndef FAT32_H__
#define FAT32_H__

#define FAT32_isEOF(x) (x >= 0x0ffffff8UL && x <= 0x0fffffffUL)
#define FAT32_EOF 0x0fffffffUL
// bpb(BIOS Parameter Block)
struct fat32hder {
  uint8_t   jmp_Boot[3];
  uint8_t   OEM_identifier[8];
  uint16_t  bytes_per_sector;
  uint8_t   sectors_per_cluster;
  uint16_t  reserved_sectors;
  uint8_t   number_fats;
  uint16_t  number_root_directory_entry;
  uint16_t  total_sectors;
  uint8_t   media_descriptor_type;
  uint16_t  number_per_fat;    // only fat12/fat16
  uint16_t  sectors_per_track;
  uint16_t  number_header;
  uint32_t  number_hidden_sector;
  uint32_t  large_sector_count;
  uint32_t  sectors_per_fat;
  uint16_t  flags;
  uint16_t  fat_version_number;
  uint32_t  cluster_root_directory;   // often set to 2
  uint16_t  FSinfo_sector_number;
  uint16_t  number_backup_boot_sector;
  uint8_t   reserved[12];
  uint8_t   derive_number;
  uint8_t   reserved_not_NT;
  uint8_t   signature;    // must be 0x28 or 0x29
  uint32_t  volume_id;
  uint8_t   volume_laboel_string[11];
  uint8_t   system_identifier_string[8];
  uint8_t   boot_code[420];
  uint16_t  bootable_partition;     // must be oxaa55
} __packed;

struct FSInfo {
  uint32_t    lead_signature;   // must be ox41615252
  uint8_t     reserved[480];
  uint32_t    another_signature;  // must be ox61417272
  uint32_t    last_known_free_cluster;
  uint32_t    hint;
  uint8_t     reserved1[12];
  uint32_t    trail_signature;
} __packed;

struct FAT32Directory {
  uint8_t   dir_name;
  uint8_t   file_attribute;
  uint8_t   dir_NTRes;
  uint8_t   dir_CrtTimeTenth;
  uint16_t  dir_CrtTime;
  uint16_t  dir_CrtDate;
  uint16_t  dir_LstAccDate;
  uint16_t  dir_FstClusHI;
  uint16_t  dir_WrtTime;
  uint16_t  dir_WrtDate;
  uint16_t  dir_FstClusLO;
  uint32_t  dir_FileSize;
} __packed;

struct FAT32LongDirectory {
  uint8_t   LDIR_Ord;
  wchar     LDIR_Name1[5];
  uint8_t   LDIR_Attr;
  uint8_t   LDIR_Type;
  uint8_t   LDIR_Chksum;
  wchar     LDIR_Name2[6];
  uint16_t  LDIR_FstClusLO;
  wchar     LDIR_Name3[2];
} __packed;

#endif // !FAT32_H__
