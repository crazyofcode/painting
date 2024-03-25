.section .text
.global _entry

_entry:
  add t0, a0, 1
  slli t0, a0, 14
  la sp, boot_stack
  add sp, sp, t0
  call main

loop:
  j loop

  .section .bss.stack
  .align 12
  .global boot_stack
boot_stack:
  .space 4096 * 4 * 2
  .global boot_stack_top
boot_stack_top:
