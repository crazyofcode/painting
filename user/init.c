#include <lib.h>
#include <stdio.h>

const char *argv[] = { "sh", 0 };

int main(int argc, char *argv[]) {
  static char buf[] = "hello world\n";
  write(STDOUT, buf, 12);
  printf("%s: starting sh\n", argv[0]);
  for (;;) {
    int pid = fork();
    if (pid < 0) {
      printf("init: exec sh failed\n");
      exit(1);
    }
    if (pid == 0) {
      exec("sh", (const char **)argv);
      printf("init: exec sh failed\n");
      exit(1);
    } else {
      exit(wait(pid));
    }
  }
}
