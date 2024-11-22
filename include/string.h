#ifndef STRING_H__

void    *memset(void *, int, uint32_t);
void    *memmove(void *, const void *, uint32_t);
void    *memcpy(void *, const void *, uint32_t);
size_t  strlen(const char *);
int     strncmp(const char *, const char *, size_t);
size_t  strncpy(char *, const char *, size_t);
int     wstr2str(char *, const char *, int);
int     str2wstr(wchar *, const char *, int);
int     fill_fat32_long_name(char *, const char *);
int     read_fat32_short_name(char *, const char *);
int     write_fat32_short_name(char *, const char *);
#endif //!STRING_H__
