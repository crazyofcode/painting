#ifndef __LIB_H__
#define __LIB_H__
#include "syscall.h"
#include "syscall-nr.h"

typedef unsigned long uint64_t;
typedef unsigned long size_t;

#define STDOUT    1

int      main(int, char **);

uint64_t write(int, char *, size_t);
uint64_t exec(const char *, const char **);
void     exit(int);
#endif //! __LIB_H__
