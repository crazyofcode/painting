#include "types.h"

void *
memset(char *addr, int c, uint size)
{
  if(!addr)
    return 0;

  char *caddr = (char *)addr;
  int i;
  for(i = 0; i < size; i++)
    *(caddr + i) = c;

  return addr;
}
