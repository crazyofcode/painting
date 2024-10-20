#ifndef TYPES_H__
#define TYPES_H__

typedef char			s8;
typedef unsigned char		u8;
typedef unsigned char		uint8_t;

typedef short			s16;
typedef unsigned short		u16;
typedef short			int16_t;
typedef unsigned short		uint16_t;

typedef int			s32;
typedef unsigned int		u32;
typedef int			int32_t;
typedef unsigned int		uint32_t;

#if __riscv_xlen == 64
typedef long			s64;
typedef unsigned long		u64;
typedef long			int64_t;
typedef unsigned long		uint64_t;
#define PRILX			"016lx"
#elif __riscv_xlen == 32
typedef long long		s64;
typedef unsigned long long	u64;
typedef long long		int64_t;
typedef unsigned long long	uint64_t;
#define PRILX			"08lx"
#else
#error "Unexpected __riscv_xlen"
#endif

typedef unsigned long		uintptr_t;
typedef unsigned long		size_t;
typedef long			ssize_t;
typedef unsigned long		virtual_addr_t;
typedef unsigned long		virtual_size_t;
typedef unsigned long		physical_addr_t;
typedef unsigned long		physical_size_t;

typedef uint64_t    pte_t;
#endif //TYPES_H__
