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

void  file_destory(const char *);

struct list *   dir_open(const char *, int);
void            dir_close(struct list *);
void            dir_create(const char *);
void            dir_destory(const char *);
#endif // !VFS_H__
