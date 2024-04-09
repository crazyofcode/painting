#ifndef __PARAM_H
#define __PARAM_H

#define UNLOCK 0
#define LOCKED 1


#define NCPU    2

#define NOMMAPFILE 60
/* lm add 4.2 */
#define NPROC        60     /* maximum number of processes*/
#define NOFILE       16      /* open files per process*/
#define NFILE        100     /* open files per system*/
#define NINODE       50      /* maximum number of active i-nodes*/
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      260   // maximum file path name
#define INTERVAL     (390000000 / 200) // timer interrupt interval

#endif