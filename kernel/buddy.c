#include "types.h"
#include "buddy.h"
#include "riscv.h"
#include "defs.h"

struct buddy *freelist[MAX_LEVEL+1];

// 计算待分配内存对应的块的等级
static inline int
_calc_level(size_t size)
{
  int sz  = MIN_LEVEL;
  size_t csz = 1 << sz;
  while(size > csz)
  {
    csz <<= 1;
    ++sz;
  }

  return sz;
}

void
buddyinit()
{
  int i;
  for(i = 0; i < MAX_LEVEL; i++)
  {
    freelist[i] = 0;
  }
}

// 对于每一个伙伴都是以偶数n索引开始,以下一个奇数 n + 1索引结束
// mask = 1 << lv;
// mask = 16 32 64 128 ... 4096
// ~mask = 15 31 63 127 ... 4095
static inline struct buddy *
_get_buddy(struct buddy *block, int lv)
{
  uint64 buddyaddr = (uint64)block ^ (1 << lv);
  return (struct buddy *)buddyaddr;
}

void *
malloc(size_t sz)
{
  sz += 8;        // 作为头部存放level和用于对齐

  int lv = _calc_level(sz);

  int i = lv;

  struct buddy *block = 0;

  for(;;i++)
  {
    if(freelist[i] != 0)
    {
      block = freelist[i];
      freelist[i] = freelist[i]->next;
      break;
    }

    if(i > MAX_LEVEL)
      break;
  }

  if(block == 0)
  {
    block = (struct buddy *)kalloc();
    if(block == 0)
      panic("buddy alloc");
  }

  struct buddy *buddy;
  for(; i > lv; i--)
  {
    buddy = _get_buddy(block, i - 1);
    buddy->next = freelist[i - 1];
    freelist[i - 1] = buddy;
  }

  // 记录该内存块的level, 给free用
  uint *b = (uint *)(block);
  *b = lv;

  return b + 8;
}

void
free(void *pa)
{
  int i = *((uint *)pa - 8);

  struct buddy *block = (struct buddy *)((uint *)pa - 8);

  struct buddy *buddy;
  struct buddy **list;

  for(;;++i)
  {
    if(i == MAX_LEVEL)
    {
      kfree(pa);
      break;
    }

    buddy = _get_buddy(block, i);

    // 判断是否存在和要释放的内存块统一级的freelist
    list = &freelist[i];

    while((list != 0) && (*list != buddy))
      list = &(*list)->next;

    if(*list != buddy)
    {
      block->next = freelist[i];
      freelist[i] = block;
      return;
    }
      else {
      block = block < buddy ? buddy : block;
      *list = (*list)->next;
    }
  }
}

// 输出mem info
void
mem_info()
{
  printf("===================================================\n");
  for(int i = MIN_LEVEL; i  < MAX_LEVEL; i++)
  {
    struct buddy *block = freelist[i];
    size_t        sz    = LEVEL_2_SIZE(i);
    printf("level %d size(%d):", i, sz);

    while(block)
    {
      printf("%p->%p, ", block, (uint64)block + sz);
      block = block->next;
    }

    printf("\n");
  }

  printf("==================================================\n");
}
