.section  trampvec
.globl trampoline
trampoline:
  .align  4
  .globl  uservec
uservec:
  wfi
userret:
  wfi
  sret

