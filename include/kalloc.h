#ifndef __KALLOC_H__
#define __KALLOC_H__

void        kmeminit(void);
void *      kalloc(void);
void        kfree(void *);

#endif  // !__KALLOC_H__
