#ifndef __BUDDY_H__
#define __BUDDY_H__

// 用于匹配需要分配块大小对应的等级,保存在每一个块的头部
#define GET_LEVEL(b)  (*((uint *)b - 1))

// 计算出该等级对应需要分配块的大小
#define LEVEL_2_SIZE(lv)  (1 << (lv))

// 最小的分配块的等级对应16字节
#define  MIN_LEVEL 4

// 最大的分配块的等级对应4096字节
#define MAX_LEVEL 12

struct buddy {
  struct buddy *next;
};

void        buddyinit();
void *      malloc(size_t);
void        free(struct buddy *);
void        mem_info();

#endif // !__BUDDY_H__
