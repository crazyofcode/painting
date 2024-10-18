.section .text
.global _entry

_entry:
        # set up a stack for C.
        # stack0 is declared in main.c,
        # with a 4096-byte stack per CPU.
        # a0: hartid
        # a1: dt entry
        la sp, stack0
        li t1, 1024*4

        addi t0, a0, 1
        mul t0, t0, t1
        add sp, sp, t0
        # jump to main() in main.c
        call main
spin:
        j spin


