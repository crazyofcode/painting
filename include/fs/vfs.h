#ifndef VFS_H__
#define VFS_H__

typedef uint32_t  mode_t;

struct file {
  uint32_t          fd;
  struct dirent *   dirent;
  off_t             pos;
  bool              deny_write;
  /*uint32_t          cluster;*/
  /*bool              removed;*/
  /*uint32_t          refcnt;*/
  struct mutex_lock lock;
  struct list_elem  elem;
};

#endif // !VFS_H__
