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

void *
memmove(void *dst, const void *src, uint32 n)
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
