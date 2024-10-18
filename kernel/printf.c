#include <stdarg.h>
#include <types.h>
#include <spinlock.h>
#include <macro.h>
#include <stdio.h>
#include <sbi.h>

#define MAXLEN  64

int putc(int ch) {
  return sbi_console_putchar(ch);
}

int puts(char *string) {
  int cnt;
  for (cnt = 0; string[cnt] != '\0'; cnt++) {
    sbi_console_putchar(string[cnt]);
  }
  return cnt;
}

static char digital[] = "0123456789abcdef";
static char NumToChar[MAXLEN];

struct {
  struct spinlock lock;
  int locked;
} pr;

static void
printstr(const char *str)
{
  int i = 0;
  for (; *(str + i); i++)
    sbi_console_putchar(*(str + i));
}

static void
printint(int xx, int base, int sign, int width)
{
  int neg = 0;
  uint32_t num;

  if (sign && xx < 0) {
    neg = 1;
    num = -xx;
  } else {
    num = xx;
  }

  int i = 0;
  do {
    NumToChar[i++] = digital[num % base];
    num /= base;
  } while(num != 0);

  if (width > 0) {
    for (; i < width - 1; i++)
      NumToChar[i] = '0';
  }

  if (neg) {
    NumToChar[i++] = '-';
  } else {
    if (width > 0)
      NumToChar[i++] = '0';
  }

  while (--i >= 0)
    sbi_console_putchar(NumToChar[i]);
}

static void
printptr(uint64_t x)
{
  int i;
  sbi_console_putchar('0');
  sbi_console_putchar('x');
  for (i = 0; i < (sizeof(uint64_t) * 2); i++, x <<= 4)
    sbi_console_putchar(digital[x >> (sizeof(uint64_t) * 8 - 4)]);
}

static void
vsprintf(va_list ap, const char *fmt)
{
  int i, c, base;
  for(i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] != '%')
    {
      sbi_console_putchar(fmt[i]);
      continue;
    }

    int align_forment = 0;

  align_forment:
    switch (c = fmt[++i]) {
      case '0' :
      case '1' :
      case '2' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '7' :
      case '8' :
      case '9' :
        align_forment += c - '0';
        goto align_forment;
      case 'c' :
        int ch = va_arg(ap, int);
        sbi_console_putchar(ch);
        break;
      case 's' :
        char *str = va_arg(ap, char *);
        printstr(str);
        break;
      case 'd' :
        base = 10;
        printint(va_arg(ap, int), base, 1, align_forment);
        break;
      case 'x' :
        base = 16;
        printint(va_arg(ap, int), base, 0, align_forment);
        break;
      case 'p' :
        printptr(va_arg(ap, uint64_t));
        break;
      default: sbi_console_putchar(c); panic("no matched");
    }
  }
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  int locked;

  locked = pr.locked;
  if (locked)
    acquire(&pr.lock);

  va_start(ap, fmt);

  vsprintf(ap, fmt);

  va_end(ap);

  if (locked)
    release(&pr.lock);
}

void
printfinit()
{
  initlock(&pr.lock, "pr");
  pr.locked = true;
}

void
panic(const char *str) {
  pr.locked = 0;
  printf("panic: ");
  printf(str);
  printf("\n");
  pr.locked = 1;
  while (1)
    ;
}
