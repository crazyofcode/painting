#ifndef VM_H__
#define VM_H__

extern pagetable_t kernel_pagetable;

void      kvminit(void);
int       mappages(pagetable_t, uint64_t, uint64_t, uint64_t, int);
void      kvminithart(void);
#endif //!VM_H__
