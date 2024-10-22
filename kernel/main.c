#include <types.h>
#include <param.h>
#include <sbi.h>
#include <memlayout.h>
#include <riscv.h>
#include <console.h>
#include <macro.h>
#include <stdio.h>
#include <pm.h>
#include <vm.h>
#include <dtb.h>
#include <trap.h>
#include <plic.h>
#include <virt.h>

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];
volatile static int started = 0;

void main(uint64_t hartid, uint64_t _dtbEntry) {
  w_tp(hartid);
  if (started == 0) {
    dtbEntryinit(_dtbEntry);
    console_init();
    printfinit();
    parseDtb();
    printf("\n");
    printf("Hello, Welcome to patingOS(hart %d):\n", hartid);
    kpminit();
    kvminit();
    kvminithart();
    trapinit();
    trapinithart();
    plicinit();
    plicinithart(hartid);
    virtio_disk_init();
    __sync_synchronize();
    started = 1;
    for (int i = 0; i < NCPU; i++) {
      if (i != hartid)
        sbi_hart_start(i, KERNBASE, 0);
    }

  } else {
    while(started == 0)
      ;
    ++started;
    __sync_synchronize();
    printf("hart %d starting\n", hartid);
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart(hartid);   // ask PLIC for device interrupts
  }

  while(started != 4)
    ;
  sbi_console_putchar(started + 48);
  sbi_shutdown();
}
