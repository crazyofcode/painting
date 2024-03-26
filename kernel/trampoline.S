.section trampsec
.global trampoline

trampoline:
.align 4

.global uservec
uservec:

  // trap.c设置trap处理程序的入口地址
  // 用户程序的trap会从这里开始
  // 跳转到此处后,已经从U->S, 但是page table仍然是S

  // 由于存储到内存需要使用gpr保存地址
  // 但是此时没有gpr可以使用
  // riscv使用sscratch提供帮助
  // 将a0保存到sscratch中,然后就可以使用a0
  csrrw a0, sscratch, a0

  // 将gpr保存到TRAPFRAME
  

  sd ra, 40(a0)
  sd sp, 48(a0)
  sd gp, 56(a0)
  sd tp, 64(a0)
  sd t0, 72(a0)
  sd t1, 80(a0)
  sd t2, 88(a0)
  sd s0, 96(a0)
  sd s1, 104(a0)
  sd a1, 120(a0)
  sd a2, 128(a0)
  sd a3, 136(a0)
  sd a4, 144(a0)
  sd a5, 152(a0)
  sd a6, 160(a0)
  sd a7, 168(a0)
  sd s2, 176(a0)
  sd s3, 184(a0)
  sd s4, 192(a0)
  sd s5, 200(a0)
  sd s6, 208(a0)
  sd s7, 216(a0)
  sd s8, 224(a0)
  sd s9, 232(a0)
  sd s10, 240(a0)
  sd s11, 248(a0)
  sd t3, 256(a0)
  sd t4, 264(a0)
  sd t5, 272(a0)
  sd t6, 280(a0)

  // 由于为了有gpr可以使用,将a0保存到了sscratch中
  // 需要从sscratch中获取a0的值保存到TRAPFRAME
  csrr t0, sscratch
  sd t0, 112(a0)


  # initialize kernel stack pointer, from p->trapframe->kernel_sp
  ld sp, 8(a0)

  # make tp hold the current hartid, from p->trapframe->kernel_hartid
  ld tp, 32(a0)

  # load the address of usertrap(), from p->trapframe->kernel_trap
  ld t0, 16(a0)


  # fetch the kernel page table address, from p->trapframe->kernel_satp.
  ld t1, 0(a0)

  # wait for any previous memory operations to complete, so that
  # they use the user page table.
  sfence.vma zero, zero

  # install the kernel page table.
  csrw satp, t1

  # flush now-stale user entries from the TLB.
  sfence.vma zero, zero

  # jump to usertrap(), which does not return
  jr t0

.global userret
.align 4
userret:
  // 在trap.c中调用
  // 由kernel切换到user
  // a1: user page table , for satp
  // a0: TRAPFRAME

  sfence.vma zero, zero
  csrw satp, a1
  sfence.vma zero, zero

  // 将a0保存到sscratch中,在恢复完寄存器的状态后
  // 把a0从sscratch中读取出来

  ld  t0, 112(a0)
  csrw  sscratch, t0


  ld ra, 40(a0)
  ld sp, 48(a0)
  ld gp, 56(a0)
  ld tp, 64(a0)
  ld t0, 72(a0)
  ld t1, 80(a0)
  ld t2, 88(a0)
  ld s0, 96(a0)
  ld s1, 104(a0)
  ld a1, 120(a0)
  ld a2, 128(a0)
  ld a3, 136(a0)
  ld a4, 144(a0)
  ld a5, 152(a0)
  ld a6, 160(a0)
  ld a7, 168(a0)
  ld s2, 176(a0)
  ld s3, 184(a0)
  ld s4, 192(a0)
  ld s5, 200(a0)
  ld s6, 208(a0)
  ld s7, 216(a0)
  ld s8, 224(a0)
  ld s9, 232(a0)
  ld s10, 240(a0)
  ld s11, 248(a0)
  ld t3, 256(a0)
  ld t4, 264(a0)
  ld t5, 272(a0)
  ld t6, 280(a0)

csrrw a0, sscratch, a0

// return to user mode and user pc
// usertrapret set up the sstatus and sepc
sret
