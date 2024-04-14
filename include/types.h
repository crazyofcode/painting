#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned long uint64;
typedef unsigned int  uint;
typedef unsigned short uint16;

typedef unsigned long size_t;
typedef unsigned char uchar;
typedef unsigned int  uint32;

typedef unsigned short ushort;
typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef uint64 pde_t;


/* add defien of NULL*/
#define NULL 0

#define readb(addr) (*(volatile uint8 *)(addr))
#define readw(addr) (*(volatile uint16 *)(addr))
#define readd(addr) (*(volatile uint32 *)(addr))
#define readq(addr) (*(volatile uint64 *)(addr))

#define writeb(v, addr)                      \
    {                                        \
        (*(volatile uint8 *)(addr)) = (v); \
    }
#define writew(v, addr)                       \
    {                                         \
        (*(volatile uint16 *)(addr)) = (v); \
    }
#define writed(v, addr)                       \
    {                                         \
        (*(volatile uint32 *)(addr)) = (v); \
    }
#define writeq(v, addr)                       \
    {                                         \
        (*(volatile uint64 *)(addr)) = (v); \
    }

#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#endif
