#ifndef PARAM_H__
#define PARAM_H__

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
#endif //!PARAM_H__
