#ifndef FILE_H__
#define FILE_H__

#include <vfs.h>

#define   SEEK_SET    0
#define   SEEK_CUR    1
#define   SEEK_END    2

#define   MAX_FILE_NAME_LEN 32
#define   MAX_FS      10

/*extern static struct filesysytem fs[MAX_FS_NUM];*/

void            init_fs(void);
struct dirent * file_open(uint64_t, int);
void            file_close(int);
bool            file_create(uint64_t, mode_t, struct dirent *);
size_t          file_read(int, uint64_t, size_t);
size_t          file_write(int, uint64_t, size_t);
void            file_seek(int, off_t, int);

struct filesystem *alloc_fs(void);
void               free_fs(struct filesystem *);
void               filesys_init(void);
#endif // !FILE_H__
