#ifndef MACRO_H__
#define MACRO_H__

#define UNUSED __attribute__((unused))
#define __packed		__attribute__((packed))
#define __noreturn		__attribute__((noreturn))
#define __aligned(x)		__attribute__((aligned(x)))

#ifndef __always_inline
#define __always_inline	inline __attribute__((always_inline))
#endif

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#ifndef __has_builtin
#define __has_builtin(...) 0
#endif

#undef offsetof
#if __has_builtin(__builtin_offsetof)
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE,MEMBER)
#elif defined(__compiler_offsetof)
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#define array_size(x) 	(sizeof(x) / sizeof((x)[0]))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, lo, hi) MIN(MAX(a, lo), hi)

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))
#define ROUNDUP(a,x)   (((a)+(x)-1) & (~((x)-1)))

#define ASSERT(condition) \
  extern void panic(const char *) __noreturn; \
  if (condition) {} \
  else { panic("assertion fail\n"); }

#define ASSERT_INFO(condition, ...) \
  if (condition) {}             \
  else { log(__VA_ARGS__); }

#define log(...)                                                                            \
	do {                                                                                       \
    extern void _log(const char *file, int line, const char *func, const char *format, ...);  \
		_log(__FILE__, __LINE__, __func__, __VA_ARGS__);                           \
	} while (0)
#endif //!MACRO_H__
