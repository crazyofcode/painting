#include <types.h>
#include <param.h>
#include <string.h>
#include <defs.h>

void *
memset(void * dst, int c, uint32 n)
{
  char *cdst = (char *)dst;

  int i;
  for (i = 0; i < n; i++) {
    *(cdst + i) = c;
  }
  return dst;
}
