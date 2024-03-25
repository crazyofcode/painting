#include "types.h"
#include "riscv.h"
#include "buddy.h"
#include "defs.h"

volatile static int started = 0;

typedef void (*function_t)();
// 在kernel.ld中提供
extern function_t __init_array_start[];
extern function_t __init_array_end[];

void
_call_global_constructor()
{
  for(function_t *fn = __init_array_start; fn < __init_array_end; fn++)
  {
    (*fn)();
  }
}

void
main(uint64 hartid, uint64 dtb_pa)
{
  inithartid(hartid);

  if(hartid == 0)
  {
    consoleinit();
    printfinit();
    _call_global_constructor();   // 初始化全局对象
    kinit();      // 初始化物理内存分配器
    buddyinit();  // 初始化通用内存分配器
    kvminit();    // 初始化内核虚拟内存
    kvminithart();  // 启用分页
    timerinit();
    binit();        // 缓冲区初始化
  }
}
