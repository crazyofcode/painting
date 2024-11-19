#ifndef VM_H__
#define VM_H__

extern pagetable_t kernel_pagetable;

void      kvminit(void);
int       mappages(pagetable_t, uint64_t, uint64_t, uint64_t, int);
void      kvminithart(void);

pagetable_t uvmcreate(void);
void        uvmunmap(pagetable_t, uint64_t, uint64_t, int);
void        freewalk(pagetable_t);
void        uvmfree(pagetable_t, uint64_t);

int         copyout(pagetable_t, uint64_t, const char *, uint64_t);
int         copyin(pagetable_t, char *, uint64_t, uint64_t);
#endif //!VM_H__
