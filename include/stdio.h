#ifndef STDIO_H__
#define STDIO_H__

#define STDIN   0
#define STDOUT  1
#define STDERR  2

int     putc(int);
int     puts(char *);
void    printf(const char *, ...);
void    printfinit();
void    panic(const char *) __noreturn;
int     sprintf(char *, const char *, ...);
#endif //!STDIO_H__
