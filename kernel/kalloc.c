#include <types.h>
#include <param.h>
#include <buddy.h>
#include <spinlock.h>

#define PACK(addr)      ((void *)((uint8_t *)addr + ALIGN))
#define UNPACK(addr)    ((void *)((uint8_t *)addr - ALIGN))
#define PROC_SIZE       (sizeof(struct proc) + ALIGN)

static size_t _fixed_size(size_t sz) {
  do {
    if (is_pow_of_2(sz))
      return sz;
    sz += sz;
  } while(1);
}

struct slab_proc {
  struct slab_proc *next;
};
struct slab_proc *slab_proc;

void slab_init(void) {
  Log("proc size: %d\n", PROC_SIZE);
  slab_proc = NULL;
}
void *kalloc(size_t size) {
  size_t sz = size + ALIGN;
  switch(sz) {
    case PROC_SIZE:
      if (slab_proc == NULL) {
        slab_proc = PACK(buddy_alloc(_fixed_size(PROC_SIZE)));
      }
      struct slab_proc *ptr = slab_proc;
      slab_proc = slab_proc->next;
      *(uint8_t *)ptr sz;
      return PACK(ptr);
    // to be added
    default:
      // 对于非专用的内存大小
      // 直接使用 buddy_alloc
      void *ptr = buddy_alloc(sz);
      return PACK(ptr);
  }
}

void *realloc(void *ptr, size_t size) {
  void *nptr = kalloc(size);
  memcpy(nptr, ptr, *(UNPACK(ptr)));
  kfree(ptr);
  return nptr;
}

void kfree(void *addr) {
  size_t sz = *(UNPACK(addr));
  switch(sz) {
    case PROC_SIZE:
      void *ptr = UNPACK(addr);
      ptr->next = slab_proc;
      slab_proc = ptr;
      break;
    default:
      void *ptr = UNPACK(addr);
      buddy_free(ptr);
  }
}
