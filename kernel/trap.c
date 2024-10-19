#include <types.h>
#include <spinlock.h>
#include <riscv.h>
#include <trap.h>
#include <schedule.h>
#include <macro.h>
#include <stdio.h>

struct spinlock tickslock;

// 在 kernelvec.S 中
void    kernelvec(void);

void trapinit(void) {
  initlock(&tickslock, "time");
}

void trapinithart(void) {
  // 将异常的向量地址写入 stvec 寄存器
  w_stval((uint64_t)kernelvec);
}

int dev_intr(void) {
  panic("todo");
  return 0;
}

void kerneltrap(void) {
  // 处理时钟中断和设备中断
  // 发生中断时, SPIE 被设置为 SIE, SIE 被设置为 0
  // Determine if the interrupt source is from S mode
  uint64_t  sepc = r_sepc();
  uint64_t  sstatus = r_sstatus();
  if ((sstatus & SIE_SPIE) == 0)
    panic("kerneltrap: the interrupt source is not from S mode");
  if ((sstatus & SIE_SSIE) != 0)
    panic("kerneltrap: interrupt enabled");

  int which_dev = 0;
  which_dev = dev_intr();
  if (which_dev == 0) {
    log("unknown source: scause: %08x, sepc: %08x, stval: %08x\n", r_scause(), r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // 如果是时钟中断
  // 当前运行的进程就会放弃 cpu 资源
  if (which_dev == 2)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}
