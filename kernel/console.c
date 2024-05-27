#include <types.h>
#include <param.h>
#include <file.h>
#include <rustsbi.h>
#include <spinlock.h>
#include <sleeplock.h>
#include <proc.h>
#include <defs.h>

struct devsw devsw[NDEV];

#define CONS_BUFF_LENGTH      0x10
#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x


// console struct
struct {
  uint32 r;
  uint32 w;
  char buf[CONS_BUFF_LENGTH];
  struct spinlock lk;
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
  uint32  target;
  char    cbuf;
  int     c;

  target = n;
  acquire(&cons.lk);
  while (n > 0) {
    // while cons.w equal to cons.r -> the cons buf is empty
    // wait until interrupt handler has put some input into cons.buf
    while (cons.w == cons.r) {
      // if current process has been killed
      // release the spinlock and return -1
      if (killed(myproc())) {
        release(&cons.lk);
        return -1;
      }

      // fall asleep without input
      sleep(&cons.r, &cons.lk);     // TODO()
    }

    c = cons.buf[cons.r++ % CONS_BUFF_LENGTH];
    if (c == C('D')) {   // end of file
      if (n < target) {
        cons.r--;
      }
      break;
    }

    cbuf = c;
    if (either_copyout(user_dst, &cbuf, dst, 1) == -1)
      break;

    dst++;
    --n;

    if (c == '\n')
      break;
  }
  release(&cons.lk);
  return target - n;
}

int
consolewrite(int user_src, uint64 src, int n) {
  int i;
  char c;

  for (i = 0; i < n; i++) {
    if (either_copyin(user_src, src+i, &c, i) == -1)
      break;
    sbi_console_putchar(c);
  }

  return i;
}

void
consoleinit()
{
  // initial console's spinlock
  initlock(&cons.lk, "cons");

  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}

