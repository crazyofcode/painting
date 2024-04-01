/*
 * create by lm. 3.31
*/

#ifndef __STRING_H
#define __STRING_H


int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);
void            wnstr(unsigned short *dst, char const *src, int len);
void            snstr(char *dst, unsigned short const *src, int len);
int             wcsncmp(unsigned short const *s1, unsigned short const *s2, int len);
char*           strchr(const char *s, char c);

#endif