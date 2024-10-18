#ifndef SBI_H__
#define SBI_H__
#include <types.h>
#include <sbi_ecall_interface.h>


struct sbiret {
  long  error;
  long  value;
};

// ext save into a7 register as extension id
// func save into a6 register as function id
static struct sbiret SBI_CALL(int ext, int fid, unsigned long arg0,
            unsigned long arg1, unsigned long arg2,
            unsigned long arg3, unsigned long arg4,
            unsigned long arg5)
{
    struct sbiret ret;

    register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);
    register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);
    register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);
    register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3);
    register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4);
    register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5);
    register uintptr_t a6 asm ("a6") = (uintptr_t)(fid);
    register uintptr_t a7 asm ("a7") = (uintptr_t)(ext);
    asm volatile ("ecall"
              : "+r" (a0), "+r" (a1)
              : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
              : "memory");
    ret.error = a0;
    ret.value = a1;

    return ret;
}

/*
// #define SBI_CALL_N(ext, funct, num, ...) \
//     SBI_CALL(ext, funct, ##__VA_ARGS__, 0, 0, 0, 0, 0, 0)
// 
// // 定义不同参数个数的SBI调用
// #define SBI_CALL_0(ext, funct)         SBI_CALL_N(ext, funct, 0)
// #define SBI_CALL_1(ext, funct, arg0)   SBI_CALL_N(ext, funct, 1, arg0)
// #define SBI_CALL_2(ext, funct, arg0, \
//                                  arg1) SBI_CALL_N(ext, funct, 2, arg0, arg1)
// #define SBI_CALL_3(ext, funct, arg0, arg1, arg2) SBI_CALL_N(ext, funct, 3, arg0, arg1, arg2)
// #define SBI_CALL_4(ext, funct, arg0, arg1, arg2, arg3) SBI_CALL_N(ext, funct, 4, arg0, arg1, arg2, arg3)
// #define SBI_CALL_5(ext, funct, arg0, arg1, arg2, arg3, arg4) SBI_CALL_N(ext, funct, 5, arg0, arg1, arg2, arg3, arg4)
// #define SBI_CALL_6(ext, funct, arg0, arg1, arg2, arg3, arg4, arg5) SBI_CALL_N(ext, funct, 6, arg0, arg1, arg2, arg3, arg4, arg5)
// */

static struct sbiret SBI_CALL_0(unsigned long ext, unsigned long funct) {
  return SBI_CALL(ext, funct, 0, 0, 0, 0, 0, 0);
}

static struct sbiret SBI_CALL_1(unsigned long ext, unsigned long funct, unsigned long args0) {
  return SBI_CALL(ext, funct, args0, 0, 0, 0, 0, 0);
}

/*static struct sbiret SBI_CALL_2(unsigned long ext, unsigned long funct, unsigned long args0,*/
/*                                unsigned long args1) {*/
/*  return SBI_CALL(ext, funct, args0, args1, 0, 0, 0, 0);*/
/*}*/

static struct sbiret SBI_CALL_3(unsigned long ext, unsigned long funct, unsigned long args0, unsigned long args1, unsigned long args2) {
  return SBI_CALL(ext, funct, args0, args1, args2, 0, 0, 0);
}

/*static struct sbiret SBI_CALL_4(unsigned long ext, unsigned long funct, unsigned long args0, unsigned long args1, unsigned long args2, unsigned long args3) {*/
/*  return SBI_CALL(ext, funct, args0, args1, args2, args3, 0, 0);*/
/*}*/
/**/
/*static struct sbiret SBI_CALL_5(unsigned long ext, unsigned long funct, unsigned long args0, unsigned long args1, unsigned long args2, unsigned long args3, unsigned long args4) {*/
/*  return SBI_CALL(ext, funct, args0, args1, args2, args3, args4, 0);*/
/*}*/
/**/
/*static struct sbiret SBI_CALL_6(unsigned long ext, unsigned long funct, unsigned long args0, unsigned long args1, unsigned long args2, unsigned long args3, unsigned long args4, unsigned long args5) {*/
/*  return SBI_CALL(ext, funct, args0, args1, args2, args3, args4, args5);*/
/*}*/



static inline void sbi_error_handler(long);
static inline struct sbiret get_spec_version() {
  return SBI_CALL_0(SBI_EXT_BASE, SBI_EXT_BASE_GET_SPEC_VERSION);
}

static inline struct sbiret sbi_get_impl_id(void) {
  return SBI_CALL_0(SBI_EXT_BASE, SBI_EXT_BASE_GET_IMP_ID);
}

static inline long sbi_console_putchar(int ch) {
  struct sbiret sbiret;
  sbiret = SBI_CALL_1(SBI_EXT_0_1_CONSOLE_PUTCHAR, SBI_EXT_DBCN_CONSOLE_WRITE, ch);

  if (sbiret.error == SBI_SUCCESS)
    return sbiret.value;
  else sbi_error_handler(sbiret.error);

  return -1;
}

static inline long sbi_console_getchar(void) {
  struct sbiret sbiret;
  sbiret = SBI_CALL_0(SBI_EXT_0_1_CONSOLE_GETCHAR, SBI_EXT_DBCN_CONSOLE_READ);

  if (sbiret.error == SBI_SUCCESS)
    return sbiret.value;
  else sbi_error_handler(sbiret.error);

  return -1;
}

static inline struct sbiret sbi_hart_start(unsigned long hartid,
                unsigned long start_addess, unsigned long opaque) {
  return SBI_CALL_3(SBI_EXT_HSM, SBI_EXT_HSM_HART_START, hartid, start_addess, opaque);
}

static inline void sbi_error_handler(long error) {
  if (error != SBI_ERR_IO) {
    // error_table[error];
    // 输出 error description
  }
  // 然后关机
  /*sbi_shutdown();*/
}
#endif // SBI_H__
