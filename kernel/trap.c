#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "sbi.h"

extern char uservec[], userret[], trampoline[];

// define in kernelvec.S
void kernelvec();

void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
  w_sstatus(r_sstatus() | SSTATUS_SIE);

  // 启用S mode的时钟中断
  w_sie(r_sie() | SIE_SEIE | SIE_SSIE | SIE_STIE);
  setTimeout();
}

// 处理用户程序的系统调用, 中断或者异常
// 在trampoline.S中调用

void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() && SSTATUS_SPP) != 0)
    panic("usertrap: not from user space");

  // 由于现在处于内核中,需要将用户程序的系统调用,中断或者异常发送到kerneltrap
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  uint64 scause = r_scause();
  // 保存用户程序的pc
  p->trapframe->epc = r_sepc();

  if(r_scause() == 8)
  {
    // syscall
    if(killed(p))
      exit(-1);

    intr_on();

    // 关闭中断,调用syscall处理用户程序的系统调用
    syscall();
  } else if (scause == 13 || scause == 15) {
    // TODO()
  } else if (scause == 3) {
    // 跳过ebreak指令
    p->trapframe->epc += 4;
    printf("ebreak %d\n", p->trapframe->a7);
  } else if ((which_dev == devintr()) != 0) {
    // ok
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    exit(-1);

  if(which_dev == 2)
    yield();

  usertrapret();
}

void
usertrapret()
{
  struct proc *p = myproc();

  // trap return 需要将kernel trap切换为user trap
  // 所以需要关闭中断,用以返回用户空间
  intr_off();

  // 设置pc为中断处理函数的入口trampoline.S的uservec()
  uint64 trampoline_uservec = TRAMPOLINE + (uint64)uservec - (uint64)trampoline;
  w_stvec(trampoline_uservec);

  p->trapframe->kernel_satp = r_satp();
  p->trapframe->sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();


  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to userret in trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64, uint64))fn)(TRAPFRAME, satp);
}

// 设备中断处理程序
// 用于判断中断来源是否为外部中断或者软件中断,并处理它
// 2: 时钟中断
// 1: 其他设备中断
// 0: 无法识别的中断
int
devintr()
{
  // 获取状态寄存器
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
    (scause & 0xff) == 9)
  {
    int irq = irq_claim();
    if(UART_IRQ == irq)
    {
      int c = sbi_console_getchar();
      if(c != -1)
      {
        consoleintr(c);
      }
    } else if (DMA0_IRQ == irq) {
      // k210
    } else if (VIRTIO_IRQ == irq) {
      virtiointr();
    } else if (irq) {
      printf("unexpected interrupt irq(%d)", irq);
    }

    if (irq) {
      plic_complete(irq);
    }

    w_sip(r_sip() & ~2);
    sbi_set_mie();

    return 1;
  } else if (0x8000000000000005L == scause) {
    clockintr();
    return 2;
  } else {
    return 0;
  }
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

