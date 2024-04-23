#ifndef     __FILE_H__
#define    __FILE_H__

struct devsw {
    int (*read)(uint64,int,int);
    int (*write)(uint64,int,int);
};

extern struct devsw devsw[];

#define     CONSOLE     1

#endif      // !__FILE_H__