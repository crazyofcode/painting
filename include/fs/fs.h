#ifndef FS_H__
#define FS_H__

#define   SEEK_SET    0
#define   SEEK_CUR    1
#define   SEEK_END    2

#define   MAX_FS      10

/*extern static struct filesysytem fs[MAX_FS_NUM];*/

void      fs_init(void);
int       file_open(const char *, int);
void      file_close(int);
int       file_create(const char *, mode_t);
size_t    file_read(int, uint64_t, size_t);
size_t    file_write(int, uint64_t, size_t);
void      file_seek(int, off_t, int);

#endif // !FS_H__f
