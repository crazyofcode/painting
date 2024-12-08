#ifndef FS_H__
#define FS_H__

bool                createItemAt(struct dirent *, const char *, struct dirent **, mode_t, bool);
struct dirent *     lookup_dirent(const char *);

#endif //!FS_H__
