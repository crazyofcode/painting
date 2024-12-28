#ifndef PM_H__
#define PM_H__

void    kpminit(void);
void    kpmfree(void *);
void    kpm_multiple_free(void *, int);
void *  kpmalloc(void);
void *  kpm_multiple_alloc(int);
#endif //!PM_H__
