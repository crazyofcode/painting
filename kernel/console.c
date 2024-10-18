#include <types.h>
#include <spinlock.h>
#include <console.h>
#include <sbi.h>
#include <macro.h>
#include <stdio.h>

#define BACKSPACE (0x100)

struct spinlock cons;
void console_init(void) {
  initlock(&cons, "cons");
  printf("console init!\n");
}

void console_putc(int ch) {
  if (ch == BACKSPACE) {
    // 处理退格
    sbi_console_putchar('\b');
    sbi_console_putchar(' ');
    sbi_console_putchar('\b');
  }
  else {
    sbi_console_putchar(ch);
  }
}
