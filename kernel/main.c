#include <types.h>
#include <param.h>
#include <rustsbi.h>
#include <kalloc.h>
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
    printfinit();
    printflogo();
    kmeminit();     // physical page allocator
    printf("hello world\n");
    kvminit();      // create kernel page table
    uint32 num = 0x00646c72;
    printf("h%x wo%s\n", 57616, &num);
    kvminithart();  // s-mode page address translation and protection
    printf("0x%08x\n", 255);
    printf("%d\n", -1);
    while (1)
      ;
  } else {
    printf("hard id -> %d\n", cpuid());
  }
}
