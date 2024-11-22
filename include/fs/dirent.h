#ifndef DIRENT_H__
#define DIRENT_H__

void            dirent_init(void);
struct dirent * dirent_alloc(void);
void            dirent_free(struct dirent *);

#endif
