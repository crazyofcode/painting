#ifndef __LIB_H__
#define __LIB_H__
#include "../../include/types.h"
#include <syscall.h>
#include <syscall-nr.h>

#define STDIN     0
#define STDOUT    1
#define STDERR    2

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int      main(int, char **);

uint64_t read(int, char *, size_t);
uint64_t write(int, char *, size_t);
uint64_t exec(const char *, char **);
void     exit(int);
int      fork(void);
int      wait(int);
void *   sbrk(uint32_t);
int      open(char *, int);
void     close(int);
int      dup(int);
int      pipe(int*);
int      chdir(const char*);
#endif //! __LIB_H__
