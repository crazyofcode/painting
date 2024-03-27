#ifndef __MMAP_H_
#define __MMAP_H_

struct vma {
  uint64 addr;
  enum {PROG, DATA} type;
  int               length;
  int               prot;
  int               flag;
  struct file *     f;
  struct inode *    ip;
  uint              offset;
} ;

void                mmap_free();
int                 LoadIfContain(pagetable_t, uint64);
struct vma *        allocvma();
void                InitVmaTable();
#endif // !__MMAP_H_
