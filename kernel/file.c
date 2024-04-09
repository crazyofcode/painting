/*
 * create by lm. 4.2
 * 文件描述符
*/

#include "param.h"
#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"
#include "stat.h"
#include "fat32.h"
#include "proc.h"    //trap.h
#include "file.h"
#include "defs.h"    //其中包含 打印、字符串等

/* 设备函数表数组定义*/
struct devsw devsw[NDEV];
/* 文件描述符表的定义*/
struct { 
    struct spinlock lock;
    struct file file[NFILE]; // 存储系统中所有打开的文件描述符
}ftable;

/* 
 * * * * * * * * * * * * * * * * * * * * * * * 
 *文件描述符初始化 
 */
void fileinit(){
  initlock(&ftable.lock, "ftable");                //初始化ftable中的自旋锁
  struct file *f;  
  for(f = ftable.file; f < ftable.file + NFILE; f++){  
    memset(f, 0, sizeof(struct file));             //遍历文件描述符表的每个元素
  }  
  /* for test*/
  printf("fileinit\n");  
     
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * 
 * 文件描述符相关的操作
 * 
*/
/*分配一个新的文件描述符*/
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return NULL;   /* 分配失败 */
}

/*增加现有文件描述符的引用计数*/
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

/*
 *  * * * * * * * * * * * * * * * * * * * 
 * 文件相关的操作
 * 
*/
/*关闭一个文件描述符，并释放与之相关的资源*/
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);  // if > 0 表明有多个进程打开此文件，不应该在此时释放资源
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;        // 表示此文件描述符不再有效
  release(&ftable.lock);

  if(ff.type == FD_PIPE){
    pipeclose(ff.pipe, ff.writable);
  } else if(ff.type == FD_ENTRY){
    eput(ff.ep);
  } else if (ff.type == FD_DEVICE) {

  }
}

/*
 *查询文件相关的信息
 * 获取 f metadata 并将其复制到用户提供的虚拟地址 addr 指向的 stat 结构体中
*/
int
filestat(struct file *f, uint64 addr)
{
  // struct proc *p = myproc();
  struct stat st;
  
  if(f->type == FD_ENTRY){
    elock(f->ep);
    estat(f->ep, &st);  /*调用estat函数获取目录项的状态信息，并存储在st中*/
    eunlock(f->ep);
    // if(copyout(p->pagetable, addr, (char *)&st, sizeof(st)) < 0)
    if(copyout2(addr, (char *)&st, sizeof(st)) < 0)
      return -1;
    return 0;
  }
  return -1;
}

/*读取文件 addr 表示数据在用户空间中的起始位置*/
int
fileread(struct file *f, uint64 addr, int n)
{
  int r = 0;

  if(f->readable == 0)
    return -1;

  switch (f->type) {
    case FD_PIPE:
        r = piperead(f->pipe, addr, n);
        break;
    case FD_DEVICE:
        if(f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
          return -1;
        r = devsw[f->major].read(1, addr, n);
        break;
    case FD_ENTRY:
        elock(f->ep);
          if((r = eread(f->ep, 1, addr, f->off, n)) > 0)
            f->off += r;
        eunlock(f->ep);
        break;
    default:
      panic("fileread");
  }

  return r;
}

/*写文件 addr 表示数据在用户空间中的起始位置*/
int
filewrite(struct file *f, uint64 addr, int n)
{
  int ret = 0;

  if(f->writable == 0)
    return -1;

  if(f->type == FD_PIPE){
    ret = pipewrite(f->pipe, addr, n);
  } else if(f->type == FD_DEVICE){
    if(f->major < 0 || f->major >= NDEV || !devsw[f->major].write)
      return -1;
    ret = devsw[f->major].write(1, addr, n);
  } else if(f->type == FD_ENTRY){
    elock(f->ep);
    if (ewrite(f->ep, 1, addr, f->off, n) == n) {
      ret = n;
      f->off += n;
    } else {
      ret = -1;
    }
    eunlock(f->ep);
  } else {
    panic("filewrite");
  }

  return ret;
}

/*获取目录中的下一个条目*/
int
dirnext(struct file *f, uint64 addr)
{
  // struct proc *p = myproc();

  if(f->readable == 0 || !(f->ep->attribute & ATTR_DIRECTORY))
    return -1;

  struct dirent de;
  struct stat st;
  int count = 0;
  int ret;
  elock(f->ep);
  while ((ret = enext(f->ep, &de, f->off, &count)) == 0) {  // skip empty entry
    f->off += count * 32;
  }
  eunlock(f->ep);
  if (ret == -1)
    return 0;

  f->off += count * 32;
  estat(&de, &st);
  // if(copyout(p->pagetable, addr, (char *)&st, sizeof(st)) < 0)
  if(copyout2(addr, (char *)&st, sizeof(st)) < 0)
    return -1;

  return 1;
}