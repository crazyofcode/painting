#ifndef UVM_H__
#define UVM_H__

uint64_t        uvmalloc(pagetable_t, uint64_t, uint64_t, int);
uint64_t        uvmdealloc(pagetable_t, uint64_t, uint64_t);
int             uvmcopy(pagetable_t, pagetable_t, uint64_t);
void            uvmfree(pagetable_t, uint64_t);
void            uvmunmap(pagetable_t, uint64_t, uint64_t, int);
void            uvmclear(pagetable_t, uint64_t);
uint64_t        walkaddr(pagetable_t, uint64_t);
pagetable_t     uvmcreate(void);

#endif // UVM_H__
