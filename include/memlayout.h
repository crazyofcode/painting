// #ifdef QEMU
// qemu puts UART register here in physical memory
#define UART  0x10000000L
#define UART_TRQ 10
// endif

// #ifdef QEMU
#define VIRTIO0 0x10001000L
#define VIRTIO_IRQ 1
// #endif // QEMU

#define PLIC    0x0c000000L
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

#define OPEN_SBI  0x80000000

// #ifdef QEMU
#define KERNBASE 0x80020000
// #endif

#define PHYSTOP 0x80600000

#define TRAMPOLINE (MAXVA - PGSIZE)

#define VKSTACK 0x3ec0000000

#define TRAPFRAME (TRAMPOLINE - PGSIZE)


#define MAXUVA   OPEN_SBI


