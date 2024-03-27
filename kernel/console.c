#include "types.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "sbi.h"

#define BACKSPACE (0x100)
#define ENTER     (13)
#define CTRL(x)   (x - '@')

struct {
  struct spinlock lock;
  
  // input
#define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;

void
consoleinit()
{
  initlock(&cons.lock, "cons");

  cons.r = 0;
  cons.w = 0;
  cons.e = 0;
}

void
consoleputc(int c)
{
  if(c == BACKSPACE)
  {
    // 处理空格
    sbi_console_putchar('\b');
    sbi_console_putchar(' ');
    sbi_console_putchar('\b');
  } else {
    sbi_console_putchar(c);
  }
}

int
consoleread(uint64 dst, int user, int n)
{
  char c;
  int expect = n;
  acquire(&cons.lock);

  while(n > 0)
  {
    // 当读的索引等于写的索引时即缓冲区的内容为空
    // 进入休眠状态
    while(cons.r == cons.w)
    {
      if(killed(myproc()))
      {
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r % INPUT_BUF_SIZE];

    if(either_copy(user, dst, &c, 1) < 0)
      break;

    ++dst;

    --n;

    if(c == '\n')
      break;
  }

  release(&cons.lock);
  return expect - n;
}

void
consoleintr(int c)
{
  panic("consoleintr todo");
}
