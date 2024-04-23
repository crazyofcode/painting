#ifndef __FS_H__
#define __FS_H__

// fat32filesystem.c
void      fat32filesystem(int);
// 创建和初始化一个 inode 节点
struct inode *allocindode();
struct inode *getrootinode();
int       create(struct inode *, const char *, int);
void      deleteinode(struct inode *);
int       updateinode(struct inode *);
int       readinode(struct inode *, int, uint64, uint64, uint);
int       writeinode(struct inode *, int, uint64, uint64, uint);
struct inode *lookup(struct inode *, const char *);
int       readdir(struct file *, struct inode *, char *, int, int);
int       mkdir(struct inode *, const char *, int);
int       unlink(struct inode *, const char *);
          // static
int       fat32init();
int       bmap(struct inode *, uint);
struct inode *getinode(uint64 pos);
uint64    findfreeentry(struct inode *, int);
struct inode *buildinode(Msdosentry *, uint64, struct inode *);
uint32    alloccluster();
void      zerocluster(uint32);
void      markentrydeleted(struct inode *, uint32, int);
int       readsector(int, char *);
int       writesector(int, char *);

// fat.c
void    FatTime2Unix(struct timespec *ts, uint16 __date, uimt16 __time);
void    UnixTime2Fat(struct timespec *ts, uint16 *date, uint16 *__time);

// syscall
void      fileclose(struct file *f);

#endif // !__FS_H__
