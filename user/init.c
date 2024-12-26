#include "lib.h"

int main(int argc, char *argv[]) {
  char buf[] = "hello world\n";
  write(STDOUT, buf, 12);
  while(1)
    ;
}
