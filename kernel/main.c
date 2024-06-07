#include <types.h>
#include <param.h>
#include <rustsbi.h>
#include <kalloc.h>
#include <defs.h>
#include <timer.h>

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
    kvminit();      // create kernel page table
    kvminithart();  // s-mode page address translation and protection
                    // S-mode turn on paging
    procinit();     // initialize process
    trapinit();     // trap vector -> timer
    traphartinit(); // install kernel trap vector
    printf("hello world\n");
    plicinit();     // platform interrupt control
    plichartinit(); // ask PLIC for device interrupts
    timerinit();    // timer interrupt
    uint32 num = 0x00646c72;
    printf("h%x wo%s\n", 57616, &num);
    printf("0x%08x\n", 255);
    printf("%d\n", -1);
    while (1)
      ;
  } else {
    printf("hard id -> %d\n", cpuid());
  }
}
