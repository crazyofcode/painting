// #ifdef QEMU
// qemu puts UART register here in physical memory
#define UART  0x10000000L
#define UART_IRQ 10
// endif

// #ifdef QEMU
#define VIRTIO0 0x10001000L
#define VIRTIO_IRQ 1
// #endif // QEMU

#define DMA0_IRQ 27

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


/*
 * changed 
 * UART ok
 * VIRTIO0 ?(L)
*/

#define VIRT_OFFSET             0x3F00000000L
#define UART_V                  (UART + VIRT_OFFSET)
#define VIRTIO0_V               (VIRTIO0 + VIRT_OFFSET)
// local interrupt controller, which contains the timer.
#define CLINT                   0x02000000L
#define CLINT_V                 (CLINT + VIRT_OFFSET)
#define PLIC                    0x0c000000L
#define PLIC_V                  (PLIC + VIRT_OFFSET)

#define PLIC_PRIORITY           (PLIC_V + 0x0)
#define PLIC_PENDING            (PLIC_V + 0x1000)
#define PLIC_MENABLE(hart)      (PLIC_V + 0x2000 + (hart) * 0x100)
#define PLIC_SENABLE(hart)      (PLIC_V + 0x2080 + (hart) * 0x100)
#define PLIC_MPRIORITY(hart)    (PLIC_V + 0x200000 + (hart) * 0x2000)
#define PLIC_SPRIORITY(hart)    (PLIC_V + 0x201000 + (hart) * 0x2000)
#define PLIC_MCLAIM(hart)       (PLIC_V + 0x200004 + (hart) * 0x2000)
#define PLIC_SCLAIM(hart)       (PLIC_V + 0x201004 + (hart) * 0x2000)

#ifndef QEMU
#define GPIOHS                  0x38001000
#define DMAC                    0x50000000
#define GPIO                    0x50200000
#define SPI_SLAVE               0x50240000
#define FPIOA                   0x502B0000
#define SPI0                    0x52000000
#define SPI1                    0x53000000
#define SPI2                    0x54000000
#define SYSCTL                  0x50440000

#define GPIOHS_V                (0x38001000 + VIRT_OFFSET)
#define DMAC_V                  (0x50000000 + VIRT_OFFSET)
#define GPIO_V                  (0x50200000 + VIRT_OFFSET)
#define SPI_SLAVE_V             (0x50240000 + VIRT_OFFSET)
#define FPIOA_V                 (0x502B0000 + VIRT_OFFSET)
#define SPI0_V                  (0x52000000 + VIRT_OFFSET)
#define SPI1_V                  (0x53000000 + VIRT_OFFSET)
#define SPI2_V                  (0x54000000 + VIRT_OFFSET)
#define SYSCTL_V                (0x50440000 + VIRT_OFFSET)


#endif
// the physical address of rustsbi
#define RUSTSBI_BASE            0x80000000

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80200000 to PHYSTOP.
#ifndef QEMU
#define KERNBASE                0x80020000
#else
#define KERNBASE                0x80200000
#endif

#define PHYSTOP                 0x80600000

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE              (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
// #define KSTACK(p)               (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE)
#define VKSTACK                 0x3EC0000000L

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME               (TRAMPOLINE - PGSIZE)

#define MAXUVA                  RUSTSBI_BASE

