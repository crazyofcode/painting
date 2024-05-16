#include <types.h>
#include <param.h>
#include <rustsbi.h>
#include <defs.h>

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// sbi 初始化后通过 entry.S 进入该函数
void
main()
{
  // 如果当前的 hart id = 0
  // 需要去初始化 S-mode 下的 OS 的环境
  if (cpuid() == 0)
  {
    consoleinit();
  }
}
