#ifndef __FILE_H__
#define __FILE_H__

// map major device number to device functio
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif // !__FILE_H__
