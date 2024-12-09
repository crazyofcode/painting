#ifndef SYSCALL_H__
#define SYSCALL_H__

// syscall.c
void        argint(uint64_t *, int);
void        argaddr(uint64_t *, int);

uint64_t      sys_getpid(void);
uint64_t      sys_create(void);
uint64_t      sys_write(void);
uint64_t      sys_read(void);
uint64_t      sys_open(void);

#endif // !SYSCALL_H__
