#ifndef VFS_H__
#define VFS_H__

typedef uint32_t  mode_t;

struct file {
  uint32_t          fd;
  struct dirent *   dirent;
  off_t             pos;
  bool              deny_write;
  mode_t            flag;
  struct list_elem  elem;
};

bool      filesys_create(uint64_t, mode_t);
int       filesys_open(uint64_t, int);
void      filesys_close(int);
size_t    filesys_read(int, uint64_t, size_t);
size_t    filesys_write(int, uint64_t, size_t);
void      filesys_seek(int, off_t, int);

#endif // !VFS_H__
