#include <types.h>
#include <param.h>
#include <file.h>
#include <defs.h>

struct devsw devsw[NDEV];
#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

void
consoleputc(int c)
{
  if (c == BACKSPACE) {
    sbi_console_putchar("\b");
    sbi_console_putchar(" ");
    sbi_console_putchar("\b");
  } else {
    sbi_console_putchar(c);
  }
}

int
consoleread(int , uint64, int);

int
consolewrite(int ,uint64, int) {
  while (1)
    consoleputc('c');
  return 0;
}

void
consoleinit()
{
  // initial console's spinlock
  // TODO()
  devsw[console].read = consoleread;
  devsw[console].write = consolewrite;
}

void
consoletest()
{
  devsw[console].write;
}
