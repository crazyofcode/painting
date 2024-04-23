#ifndef __FAT32_FILE_SYSTEM_H_
#define __FAT32_FILE_SYSTEM_H_

const uint8_t KMsdosRootIno = 1;    // MINIX_ROOT_INO
const uint16_t KMaxszOFName = 30;

struct Fat32Info {
  uint32_t    first_data_sector_;
  uint32_t    cluster_count_;
  uint16_t    bytes_per_cluster;
  uint16_t    bytes_per_sector_;
  uint16_t    sector_per_fat_;
  uint8_t     fat_num_;
  uint_t      sector_per_cluster_;
  uint16_t    reserve_sector_;
  uint32_t    root_directory_cluster_start_;
  struct MsdosInodeInfo   root_;
};

inline uint32 fatsectorofcluster(uint32);
inline uint32 fatoffsetofcluster(uint32);
inline uint32 firstsectorofcluster(uint32);
inline int    setdatentry(uint32, uint32);
inline uint32 getfatentry(uint32);
inline void   freeclusterchain(uint32);
inline int    writecluster(uint32, int, uint64, int);
inline int    readcluster(uint32, int, uint64, int);

#endif // !__FAT32_FILE_SYSTEM_H_

