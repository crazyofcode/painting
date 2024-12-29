#ifndef VM_H__
#define VM_H__

extern pagetable_t kernel_pagetable;

void        kvminit(void);
int         mappages(pagetable_t, uint64_t, uint64_t, uint64_t, int);
void        kvminithart(void);

void        freewalk(pagetable_t);

int         copyout(pagetable_t, uint64_t, const char *, uint64_t);
int         copyin(pagetable_t, char *, uint64_t, uint64_t);

pte_t *     walk(pagetable_t, uint64_t, int);

void        vmprint(pagetable_t, int);
#endif //!VM_H__
