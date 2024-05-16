#include <types.h>
#include <param.h>
#include <file.h>
#include <rustsbi.h>
#include <spinlock.h>
#include <defs.h>

struct devsw devsw[NDEV];

#define MLEN      0x10
#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x


// console struct
struct {
  int r,
  int w,
  char buf[MLEN];
  struct spinlock lk;   //  TODO()
} cons;

void
consoleputc(int c)
{
  if (c == BACKSPACE) {
    sbi_console_putchar('\b');
    sbi_console_putchar(' ');
    sbi_console_putchar('\b');
  } else {
    sbi_console_putchar(c);
  }
}

int
consoleread(int user_dst, uint64 dst, int n) {
  // TODO()
  return 0;
}

int
consolewrite(int user_src, uint64 src, int n) {
  // TODO()
  return 0;
}

void
consoleinit()
{
  // initial console's spinlock
  initlock(&cons.lk, "cons");   // TODO()

  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}

