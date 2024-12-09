#ifndef FS_H__
#define FS_H__

#define MAX_LONGENT     8
struct long_entry_set {
	struct FAT32LongDirectory *longEnt[MAX_LONGENT];
	int cnt;
};

bool                createItemAt(struct dirent *, const char *, struct dirent **, mode_t, bool);
struct dirent *     get_file(struct dirent *, const char *);

#define E_BAD_PATH    1
#define E_NOT_FOUND   2
#define E_EXCEED_FILE 3
#endif //!FS_H__
