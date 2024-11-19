#ifndef DEFS_H__

struct cpu *    cur_cpu(void);
struct proc*    cur_proc(void);
void            process_init(void);
struct proc*    process_create(void);
bool            process_execute(const char *, const char **);
pid_t           pid_alloc(void);
void            pid_free(pid_t);
pid_t           fork(void);
void            init_first_proc(void);
int             getpid();
void            sleep(void *, struct spinlock*);
void            wakeup(void *);
bool            loader(const char *);
void            sched(void);

// kalloc.c
void            slab_init(void);
void *          kalloc(size_t, mode_t);
void            kfree(void *, mode_t);
void *          rekalloc(void *, size_t, mode_t);
#endif //!DEFS_H__
