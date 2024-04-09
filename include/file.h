/*
 * Create by lm. 4.2
*/

#ifndef __FILE_H
#define __FILE_H

/* 文件描述符结构体 */
struct file {
  enum { FD_NONE, FD_PIPE, FD_ENTRY, FD_DEVICE } type;  //管道（pipe）、文件系统中的条目（entry）、设备（device）或者无（none）
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct dirent *ep; //表示文件系统中的目录条目
  uint off;          // FD_ENTRY偏移量在文件中的当前读取/写入位置
  short major;       // FD_DEVICE主设备号用于标识设备类型
};

// #define major(dev)  ((dev) >> 16 & 0xFFFF)
// #define minor(dev)  ((dev) & 0xFFFF)
// #define	mkdev(m,n)  ((uint)((m)<<16| (n)))

// 设备函数表用于读取和写入设备
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
/* 文件描述符相关函数 */
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, uint64, int n);
int             filestat(struct file*, uint64 addr);
int             filewrite(struct file*, uint64, int n);
int             dirnext(struct file *f, uint64 addr);

#endif