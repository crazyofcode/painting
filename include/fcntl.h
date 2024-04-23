#ifndef  __FCNTL_H_
#define __FCNTL_H_

#define O_RDONLY 0x001
#define O_WRONLY 0x002
#define RDWR     0x002


#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02

//      mmap

/*************fstat******************/
#define         __S_IFMT    0170000         /*these bits determine  file type*/

/***file types***/
#define         __S_IFDIR    0040000
#define         __S_IFCHR   0020000
#define         __S_IFBLK   0060000
#define         __S_IFREG   0100000
#define         __S_IFFIFO  0010000
#define         __S_IFINK    0120000
#define         __S_IFSOCK  0140000
/***file    types***/    

#define __S_ISTYPE(mode, mask)  (((mode) & __S_IFMT) == (mask))

#define S_ISDIR(mode)       __S_ISTYPE((mode), __S_IFDIR)

#endif
