#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

size_t
strlen(const char *str)
{
  size_t n;

  for (n = 0; str[n]; n++) 
    ;

  return n;
}

char *
strcpy(char *dst, const char *src)
{
  size_t i = 0;
  for(; *(src + i); i++)
    *(dst + i) = *(src + i);

  return dst;
}

char *
strncpy(char *dst, const char *src, size_t len)
{
  size_t i;

  for (i = 0; *(src + i) && i < len; i++) {
    *(dst + i) = *(src + i);
  }

  return dst;
}

int
strcmp(const char *s1, const char *s2)
{
  size_t i;

  for(i = 0; s1[i] || s2[i]; i++)
  {
    if(s1[i] != s2[i])
      return s1[i] - s2[i];
  }

  return 0;
}

int
strncmp(const char *s1, const char *s2, size_t len)
{
  size_t i;

  for(i = 0; (s1[i] || s2[i]) && i < len; i++)
  {
    if(s1[i] != s2[i])
      return s1[i] - s2[i];
  }

  return 0;
}


char *
strcat(char *dst, const char *src) {
  // panic("Not implemented");
  size_t dst_len = strlen(dst);
  size_t i;

  for(i = 0; src[i] != '\0'; i++)
  {
    dst[dst_len + i] = src[i];
  }

  dst[dst_len + i] = '\0';

  return dst;
}

void *
memset(void *dst, int c, size_t len)
{
  size_t i;

  for(i = 0; i < len; i++)
    *(dst + i) = c;

  return dst;
}

void *
memmove(void *dst, const void *src, size_t n) {
  // panic("Not implemented");
  const char *s;
  char *d;

  if(n == 0)
    return dst;
  s = src;
  d = dst;

  if(s < d && s + n > d)
  {
    s += n;
    d += n;

    while(n-- > 0)
      *--d = *--s;
  } else {
    while(n-- > 0)
      *d++ = *s++;
  }
  return dst;
}

void *
memcpy(void *out, const void *in, size_t n) {
  // panic("Not implemented");
  /*
  size_t i;

  char *cout = (char *)out;
  const char *cin = (char *)in;
  for(i = 0; i < n; i++)
  {
    *(cout + i) = *(cin + i);
  }

  return out;
  */

  return memmove(out, in, n);
}

int 
memcmp(const void *s1, const void *s2, size_t n) {
  // panic("Not implemented");
  if(n == 0)
    return 0;

  const char *cs1 = (char *)s1;
  const char *cs2 = (char *)s2;
  size_t i;
  for(i = 0; i < n; i++)
  {
    if(*(cs1 + i) != *(cs2 + i))
      return *(cs1 + i) - *(cs2 + i);
  }

  return 0;
}
