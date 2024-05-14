.section .text
.global _entry

_entry:
        # set up a stack for C.
        # stack0 is declared in main.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li a0, 1024*4
        # csrr a1, mhartid
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
        # jump to main() in main.c
        call main
spin:
        j spin



