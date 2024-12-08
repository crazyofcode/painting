#ifndef SYSCALL_H__
#define SYSCALL_H__

// syscall.c
uint64_t      argint(int);
uint64_t      argaddr(int);

uint64_t      sys_getpid(void);
uint64_t      sys_create(void);
uint64_t      sys_write(void);
uint64_t      sys_read(void);

#endif // !SYSCALL_H__
