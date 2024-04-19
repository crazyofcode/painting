/*
 * changed
 * BSIZE 512      size of sector and defined other place.
 * NBUF 30
*/

#ifndef __PARAM_H
#define __PARAM_H

#define UNLOCK 0
#define LOCKED 1

#define NPROC       64
#define NCPU        2
#define NOFILE      16     // open files per process
#define NFILE       100    // open files per system
#define NINODE      50     // maximum number of active i-nodes
#define NDEV        10     // maximum major device number

#define ROOTDEV       1    // device number of file system root disk
#define MAXARG       32    // max exec arguments
#define MAXOPBLOCKS  10    // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks  /* before 2000*/
#define MAXPATH      260   // maximum file path name  /* before 128 */
#define INTERVAL     (390000000 / 200) // timer interrupt interval /* 时钟频率 ？*/

#define NOMMAPFILE 60      //unknown


#endif