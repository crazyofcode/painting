#ifndef STRING_H__ 
#define STRING_H__

void    *memset(void *, int, uint32_t);
void    *memmove(void *, const void *, uint32_t);
void    *memcpy(void *, const void *, uint32_t);
size_t  strlen(const char *);
int     strncmp(const char *, const char *, size_t);
size_t  strncpy(char *, const char *, size_t);
size_t  gets(char *, int);
char*   strchr(const char*, char c);

#endif // !STRING_H__ 
