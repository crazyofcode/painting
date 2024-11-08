#include <types.h>
#include <param.h>
#include <pm.h>
#include <buddy.h>

#define _get_buddy(block, level)      \
            (struct buddy *)((uint64_t)block ^ (1 << level));

struct buddy {
  struct buddy  *next;
};

struct buddy *buddy_list[MAX_LEVEL];

size_t next_pow_of_2(size_t nbytes) {
  if (is_pow_of_2(nbytes))
    return nbytes;
  nbytes |= nbytes >> 1;
  nbytes |= nbytes >> 2;
  nbytes |= nbytes >> 4;
  nbytes |= nbytes >> 8;
  nbytes |= nbytes >> 16;
  nbytes |= nbytes >> 32;

  return nbytes+1;
}

static uint8_t level_of_bytes(size_t nbytes) {
  uint8_t level = 0;
  while (nbytes)
    ++level, nbytes = nbytes >> 1;
  return level;
}

void buddy_init(void) {
  for (uint8_t i = 0; i < MAX_LEVEL; i++) {
    buddy_list[i] == 0;
  }
}

/**
* 使用 slab 分配器实际分配
* buddy 只是用于为 slab 分配器提供内存页面
* pack 和 unpack 的过程会在 slab 实现的函数中体现
*/

void *buddy_alloc(size_t sz) {
  ASSERT_INFO(is_pow_of_2(sz), "sz: %d is not pow of 2\n", sz);

  uint8_t i;
  uint8_t level = level_of_bytes(sz);
  struct buddy *block = NULL;
  for (i = level; i < MAX_LEVEL; ++i) {
    if (buddy_list[i] != NULL) {
      block = buddy_list[i];
      buddy_list[i] = buddy_list[i]->next;
      break;
    }
  }

  if (block == NULL) {
    block = (struct buddy *)kpmalloc();
    ASSERT_INFO(block == NULL, "buddy alloc page fault\n");
  }

  struct buddy *buddy;
  for (i = MAX_LEVEL - 1; i > lv; i--) {
    buddy = _get_buddy(block, i);
    buddy->next = buddy_list[i];
    buddy_list[i] = buddy;
  }

  uint8_t *b = (uint8_t)block;
  *b = level;
  return (void *)b;
}

void buddy_free(void *addr) {
  struct buddy *block = (struct buddy *)addr;
  uint8_t       i = *(uint8_t *)addr;

  struct buddy *buddy;
  struct buddy **list;
  for (;; ++i) {
    if (i == MAX_LEVEL) {
      kpmfree(addr);
      break;
    }

    buddy = _get_buddy(block, i);

    list = &buddy_list[i];
    while((*list != NULL) && (*list != buddy))
      list = &(*list)->next;

    if (*list != buddy) {
      block->next = buddy_list[i];
      buddy_list[i] = block;
      return;
    } else {
      block = MIN(block, buddy);
      *list = (*list)->next;
    }
  }
}
