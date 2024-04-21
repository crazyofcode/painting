#include "types.h"
// #include "stat.h"
#include "user.h"

#include <stdarg.h>

#define NBUF 16
static char digital[] = "0123456789abcdef";
static char null[] = "(null)";

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  char buf[NBUF];
  int x;
  uint nge = 0;

  // 判断是否为复数,且作为有符号数输出
  if (sgn && xx < 0)
    x = -xx, nge = 1;
  else
    x = xx;

  int i = 0;
  do {
    buf[i++] = digital[x % base];
    x /= base;
  } while(x > 0);

  if (nge == 1)
    buf[i++] = '-';

  while (i-- >= 0)
    putc(fd, buf[i]);
}

static void
printptr(int fd, uint64 x) {
  int i;
  putc(fd, '0');
  putc(fd, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digital[x >> (sizeof(uint64) * 8 - 4)]);
}

static void
printstr(int fd, const char *str)
{
  int i;
  if (str == 0)
  {
    for(i = 0; null[i]; i++)
      putc(fd, null[i]);
  } else {
    for(i = 0; *(str + i); i++)
      putc(fd, *(str + i));
  }
}

static int
vprintf(int fd, const char *fmt, va_list ap)
{
  int i;

  for(i = 0; fmt[i]; i++)
  {
    if (fmt[i] != '%') {
      putc(fd, fmt[i]);
      continue;
    }
    switch (fmt[++i]) {
      case 'c':
        putc(fd, va_arg(ap, uint));
        break;
      case 's':
        const char *str = va_arg(ap, char *);
        printstr(fd, str);
        break;
      case 'd':
        printint(fd, va_arg(ap, int), 10, 1);
        break;
      // case 'u':
      //   printint(fd, va_arg(ap, uint), 10, 0);
      //   break;
      case 'l':
        printint(fd, va_arg(ap, uint64), 10, 0);
        break;
      case 'x':
        printint(fd, va_arg(ap, uint64), 16, 0);
        break;
      case 'p':
        printptr(fd, va_arg(ap, uint64));
        break;
      case '%':
        putc(fd, '%');
      default:
        putc(fd, '%');
    }
  }

  return i;
}

int
printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  return vprintf(1, fmt, ap);
}
