#include <types.h>
#include <string.h>

void * memset(void * dst, int c, uint32_t n) {
  char *cdst = (char *)dst;

  int i;
  for (i = 0; i < n; i++) {
    *(cdst + i) = c;
  }
  return dst;
}

void * memmove(void *dst, const void *src, uint32_t n) {
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
