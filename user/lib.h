#ifndef __LIB_H__
#define __LIB_H__
#include "syscall.h"
#include "syscall-nr.h"

typedef unsigned long uint64_t;
typedef unsigned long size_t;

#define STDOUT    1

uint64_t write(int, char *, size_t);
#endif //! __LIB_H__
