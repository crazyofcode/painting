#ifndef FILE_H__
#define FILE_H__

#include "../../user/include/fcntl.h"

#define   SEEK_SET    0
#define   SEEK_CUR    1
#define   SEEK_END    2

#define   MAX_FS      10

void            init_fs(void);
struct dirent * file_open(struct dirent *, char *, int);
bool            file_close(struct dirent *);
bool            file_create(struct dirent *, char *, mode_t, struct dirent *);
size_t          file_read(struct dirent *, char *, off_t, size_t);
size_t          file_write(struct dirent *, char *, off_t, size_t);
void            file_seek(int, off_t, int);
bool            file_remove(struct dirent *, char *);

struct filesystem *alloc_fs(void);
void               free_fs(struct filesystem *);
void               filesys_init(void);

int             alloc_fd(void);
void            free_fd(int);

struct filesystem *get_root_fs(void);

#endif // !FILE_H__
