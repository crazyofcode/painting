#include "types.h"

void *
memset(void *addr, int c, uint size)
{
  if(!addr)
    return 0;

  char *caddr = (char *)addr;
  int i;
  for(i = 0; i < size; i++)
    *(caddr + i) = c;

  return addr;
}

void*
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  if(n == 0)
    return dst;
  
  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}
