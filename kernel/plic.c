#include <types.h>
#include <plic.h>
#include <memlayout.h>

void plicinit(void) {
  // 将中断的优先级设置为非0
  // 开启 VIRIIO 的中断
  *(uint32_t *)(PLIC + VIRTIO0_IRQ * 4) = 1;
  // 开启 UART 的中断
  *(uint32_t *)(PLIC + UART0_IRQ * 4)   = 1;
}

void plicinithart(uint64_t hartid) {
  // enable bits for source ... contexts
  *(uint32_t *)PLIC_SENABLE(hartid) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // Claim/complete for contexts
  // 是否有必要
  *(uint32_t *)(PLIC_SCLAIM(hartid)) = 0;
  // 允许所有优先级的中断
  *(uint32_t *)PLIC_STHRESHOLD(hartid) = 0;
}
