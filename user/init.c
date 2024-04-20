#include "user.h"

char *argv[] = {"sh", 0};

int main(void)
{
  int pid;
  int wpid;

  open("/dev/tty", O_RDWR);

  dup(0);   // stdout
  dup(0);   // stderr

  for(;;)
  {
    printf("init: starting sh\n");
    pid = fork();

    if(pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }

    if(pid == 0)
    {
      execve("shell", argv, 0);   // exec 类的函数不会返回
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;) {
      wpid = wait((int *) 0);

      if(wpid == pid) {
        // while (1)
        //   ;

        break;
      }

      else if (wpid < 0) {
        printf("init: wait returned an error\n");

        exit(1);
      }

      else {
        // do nothing it was a parentless process;
      }
    }
  }
}
