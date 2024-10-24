#ifndef DEFS_H__

struct cpu *    cur_cpu(void);
struct proc*    cur_proc(void);
void            process_init(void);
struct proc*    process_create(void);
pid_t           pid_alloc(void);
void            pid_free(pid_t);
pid_t           fork(void);
#endif //!DEFS_H__
