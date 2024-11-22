#ifndef PARAM_H__
#define PARAM_H__

#define FATDIR_DLT  0x20
#define MAXARG    32
#define NCPU      4
#define TID_ERROR 0xffffffff
#define MAXLEN    64
#define MAX_PID   32768 // 假设最大PID号为32768
#define EM_X86_64 0x3e
#define EM_I386   0x03
#define EM_RISCV  0xf3
#define EM_MIPS   0x08

#define MAXOPBLOCKS 10        // max # of blocks any FS op writes
#define LOGSIZE (MAXOPBLOCKS * 3)        // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS * 3)           // size of disk block cache

#define ERR       0xffffffff

#define PGMASK    ((1UL << 12) - 1)

// kalloc mode
#define     PROC_MODE           (1 << 1)
#define     TRAPFRAME_MODE      (1 << 2)
#define     RB_MODE             (1 << 3)
#define     DEFAULT             (1 << 4)

#define true			1
#define false			0
#endif //!PARAM_H__
