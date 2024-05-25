#ifndef __PARAM_H__
#define __PARAM_H__

#define      NCPU       4
#define      NPROC      64
#define      NDEV       10

#define      NULL       ((void *)0)
// spinlock
#define   LOCK    1
#define   UNLOCK  0

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user can access

#endif // !__PARAM_H__
