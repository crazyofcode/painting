#include <types.h>
#include <param.h>
#include <buddy.h>
#include <spinlock.h>
#include <list.h>
#include <proc.h>
#include <macro.h>
#include <string.h>
#include <defs.h>

#define FILL_INFO       40
#define WSIZE           4
#define PGSIZE          4096
#define PROC_SIZE       ((sizeof(struct proc)) + 2 * ALIGN)
#define TRAPFRAME_SIZE  ((sizeof(struct trapframe)) + 2 * ALIGN)
#define PACK(addr)      ((void *)((uint8_t *)addr + ALIGN))
#define UNPACK(addr)    ((void *)((uint8_t *)addr - ALIGN))
#define GET_SIZE(p)     (*((uint32_t *)(p)))
#define GET_PREV_SIZE(p)(*((uint32_t *)(p) - WSIZE))
#define GET_ALLOC(p)    (*((uint32_t *)(p)+WSIZE))
#define HDRP(bp)        ((char *)(bp) - ALIGN)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - 2 * ALIGN)
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_PREV_SIZE(HDRP(bp)))
#define PUT(ptr, val)   (*(size_t *)(ptr) = val)

static char *proc_heap_list;
static char *trapframe_heap_list;

// 在对应的 list 找到空闲的空间
static void *find_fit(char *list, size_t sz) {
  void *bp;
  for (bp = list; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if ((!GET_ALLOC(HDRP(bp))) && GET_SIZE(HDRP(bp)) >= sz)
      return bp;
  }
  return NULL;
}

static void place(void *bp, size_t sz) {
  size_t csize = GET_SIZE(HDRP(bp));

  if ((csize - sz) >= 2*ALIGN) {
    PUT(HDRP(bp), sz);
    PUT(HDRP(bp)+WSIZE, 1);
    PUT(FTRP(bp), sz);
    PUT(FTRP(bp)+WSIZE, 1);

    bp = NEXT_BLKP(bp);

    PUT(HDRP(bp), csize - sz);
    PUT(HDRP(bp)+WSIZE, 0);
    PUT(FTRP(bp), csize - sz);
    PUT(FTRP(bp)+WSIZE, 0);
  } else {
    PUT(HDRP(bp), csize);
    PUT(HDRP(bp)+WSIZE, 1);
    PUT(FTRP(bp), csize);
    PUT(FTRP(bp)+WSIZE, 1);
  }
}

static void heap_list_init(void **heap_list, void *prev) {
  void *ptr = PACK(buddy_alloc(PGSIZE));
  PUT(ptr, (uint64_t)prev);
  if (prev != NULL)
    PUT(prev + PGSIZE - FILL_INFO - ALIGN, (uint64_t)ptr);
  *heap_list = PACK(PACK(ptr));
  PUT(HDRP(*heap_list), PGSIZE - FILL_INFO);
  PUT(HDRP(*heap_list) + WSIZE, 0);
  PUT(FTRP(*heap_list), PGSIZE - FILL_INFO);
  PUT(FTRP(*heap_list) + WSIZE, 0);
}

static void coalesce(void *ptr, void *addr) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(addr)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(addr)));
  size_t size = GET_SIZE(HDRP(addr));

  if (prev_alloc && next_alloc) {
    return;
  } else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(addr)));
    PUT(HDRP(addr), size);
    PUT(HDRP(addr)+WSIZE, 0);
    PUT(FTRP(addr), size);
    PUT(FTRP(addr)+WSIZE, 0);
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(addr)));
    PUT(HDRP(addr), size);
    PUT(HDRP(addr)+WSIZE, 0);
    PUT(FTRP(addr), size);
    PUT(FTRP(addr)+WSIZE, 0);
  } else {
    size += (GET_SIZE(HDRP(NEXT_BLKP(addr))) + GET_SIZE(FTRP(PREV_BLKP(addr))));
    if (size == PGSIZE - FILL_INFO) {
      addr = PREV_BLKP(addr);
      goto free_page;
    } else {
      PUT(HDRP(addr), size);
      PUT(HDRP(addr)+WSIZE, 0);
      PUT(FTRP(addr), size);
      PUT(FTRP(addr)+WSIZE, 0);
    }
  }
  return;
free_page:
  void *prev_ptr = UNPACK(UNPACK(addr));
  void *next_ptr = addr + PGSIZE - FILL_INFO - ALIGN;
  PUT(UNPACK(next_ptr), (uint64_t)prev_ptr);
  buddy_free(UNPACK(UNPACK(UNPACK(addr))));
}

void slab_init(void) {
  log("proc size: %d\n", PROC_SIZE);
  log("trapframe size: %d\n", TRAPFRAME_SIZE);
  heap_list_init((void **)&proc_heap_list, 0);
  heap_list_init((void **)&trapframe_heap_list, 0);
}
void *kalloc(size_t size, mode_t mode) {
  void *ptr = NULL;
  void *bp = NULL;
  switch(mode) {
    case PROC_MODE:
      if ((bp = find_fit(proc_heap_list, PROC_SIZE)) == NULL) {
        heap_list_init(&ptr, proc_heap_list);
        proc_heap_list = ptr;
        bp = ptr;
      }
      place(bp, PROC_SIZE);
      return bp;
    case TRAPFRAME_MODE:
      if ((bp = find_fit(trapframe_heap_list, PROC_SIZE)) == NULL) {
        heap_list_init(&ptr, (void *)trapframe_heap_list);
        trapframe_heap_list = ptr;
        bp = ptr;
      }
      place(bp, PROC_SIZE);
      return bp;
    default:
      // 对于非专用的内存大小
      // 直接使用 buddy_alloc
      size_t sz = next_pow_of_2(size);
      ptr = buddy_alloc(sz);
      return PACK(ptr);
  }
}

void *realloc(void *ptr, size_t size, mode_t mode) {
  void *nptr = kalloc(size, mode);
  memcpy(nptr, ptr, GET_SIZE(HDRP(ptr)));
  kfree(ptr, mode);
  return nptr;
}

void kfree(void *addr, mode_t mode) {
  if (addr == NULL) {
    log("kfree: addr is NULL\n");
    return;
  }
  void *ptr;
  switch (mode) {
    case PROC_MODE:
      ptr = proc_heap_list;
      goto do_free;
    case TRAPFRAME_MODE:
      ptr = trapframe_heap_list;
      goto do_free;
    default:
      void *ptr = UNPACK(addr);
      buddy_free(ptr);
      return;
  }
do_free:
  size_t sz = GET_SIZE(HDRP(addr));
  PUT(HDRP(addr), sz);
  PUT(HDRP(addr)+WSIZE, 0);
  PUT(FTRP(addr), sz);
  PUT(FTRP(addr)+WSIZE, 0);
  coalesce(ptr, addr);
}
