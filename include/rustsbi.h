#ifndef  __RUSTSBI_H__

// rust-sbi function call number
#define     set_timer               0
#define     console_putchar         1
#define     console_getchar         2
#define     clear_ipi               3
#define     send_ipi                4
#define     remote_fence_i          5
#define     remote_sfence_vma       6
#define     remote_sfence_vma_asid  7
#define     shutdown                8

// rust-sbi documents
#define SBI_CALL(funct, arg0, arg1, arg2, arg3) ({ \
  register  uintptr_t a0 asm  ("a0") = (uintptr_t)(arg0); \
  register  uintptr_t a1 asm  ("a1") = (uintptr_t)(arg1); \
  register  uintptr_t a2 asm  ("a2") = (uintptr_t)(arg2); \
  register  uintptr_t a3 asm  ("a3") = (uintptr_t)(arg3); \
  register  uintptr_t a7 asm  ("a7") = (uintptr_t)(funct);  \
  asm volatile ("ecall" \
      : "+r" (a0) \
      : "r" (a1), "r" (a2), "r" (a3), "r" (a7) \
      : "memory"); \
    a0; \
})

#define SBI_CALL_0(funct)            SBI_CALL(funct, 0, 0, 0, 0)
#define SBI_CALL_1(funct, arg0)      SBI_CALL(funct, arg0, 0, 0, 0)
#define SBI_CALL_2(funct, arg0, arg1) \
                                     SBI_CALL(funct, arg0, arg1, 0, 0)
#define SBI_CALL_3(funct, arg0, arg1, arg2) \
                                     SBI_CALL(funct, arg0, arg1, arg2, 0)
#define SBI_CALL_4(funct, arg0, arg1, arg2, arg3) \
                                     SBI_CALL(funct, arg0, arg2, arg2, arg3)

static inline int sbi_console_getchar(void) {
  return SBI_CALL_0(console_getchar);
}

static inline void sbi_console_putchar(int ch) {
  SBI_CALL_1(console_putchar, ch);
}

static inline void sbi_set_timer(uint64 t)
{
  SBI_CALL_1(set_timer, t);
}

#endif // ! __RUSTSBI_H__
