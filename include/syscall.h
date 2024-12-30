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
uint64_t      sys_exit(void) __noreturn;
uint64_t      sys_exec(void);
uint64_t      sys_close(void);
uint64_t      sys_dup(void);
uint64_t      sys_pipe(void);
uint64_t      sys_chdir(void);
uint64_t      sys_sbrk(void);
uint64_t      sys_fork(void);
uint64_t      sys_wait(void);

void          syscall(void);
#endif // !SYSCALL_H__
