#include <types.h>
#include <plic.h>
#include <riscv.h>
#include <memlayout.h>

void plicinit(void) {
  // 将中断的优先级设置为非0
  // 开启 VIRIIO 的中断
  *(uint32_t *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

void plicinithart(uint64_t hartid) {
  // enable bits for source ... contexts
  *(uint32_t *)PLIC_SENABLE(hartid) = (1 << VIRTIO0_IRQ);

  // 允许所有优先级的中断
  *(uint32_t *)PLIC_STHRESHOLD(hartid) = 0;
}

int plic_claim(void) {
  int hartid = r_tp();
  int irq = *(uint32_t *)PLIC_SCLAIM(hartid);
  return irq;
}

void plic_complete(int irq) {
  int hartid = r_tp();
  *(uint32_t *)PLIC_SCLAIM(hartid) = irq;
}
