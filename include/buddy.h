// Reference: https://github.com/cloudwu/buddy
// has been modifyed.

#ifndef BUDDY_H__
#define BUDDY_H__

#define is_pow_of_2(x)  (!(x & (x-1)))
#define MAX_LEVEL     12
#define ALIGN         8

void        buddy_init(void);
void*       buddy_alloc(size_t);
void        buddy_free(void *);
size_t      next_pow_of_2(size_t);
#endif
