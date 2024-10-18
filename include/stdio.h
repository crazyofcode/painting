#ifndef STDIO_H__
#define STDIO_H__

int     putc(int);
int     puts(char *);
void    printf(const char *, ...);
void    printfinit();
void    panic(const char *) __noreturn;
#endif //!STDIO_H__
