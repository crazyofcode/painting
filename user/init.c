#include "lib.h"

int main() {
  char buf[] = "hello world\n";
  write(STDOUT, buf, 12);
  while(1)
    ;
}
