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

void *memcpy(void *dst, const void *src, uint32_t n) {
  return memmove(dst, src, n);
}

size_t strlen(const char *s) {
  size_t len = 0;
  while(*(s + len) != '\0')
    ++len;
  return len;
}

int strncmp(const char *s1, const char *s2, size_t len) {
  size_t idx = 0;
  while (*(s1 + idx) != '\0' && *(s2 + idx) != '\0' && idx < len) {
    if (*(s1 + idx) != *(s2 + idx))
      return *(s1 + idx) - *(s2 + idx);
    ++idx;
  }

  if (idx >= len)
    return 0;
  else if (*(s1 + len) != '\0')
      return *(s1 + idx);
  else return *(s2 + idx);
}

size_t strncpy(char *dst, const char *src, size_t n) {
  if(n <= 0)
    return 0;
  size_t idx = 0;
  size_t sn = n;
  while(n-- > 0 && (*(dst + idx) = *(src + idx)) != 0) {
    ++idx;
  }
  *(dst + idx) = 0;
  return sn - n;
}

char* strchr(const char* s, char c) {
  for (; *s; s++) {
    if (*s == c)
      return (char *)s;
  }

  return NULL;
}

size_t gets(char *buf, int size) {
  int i;
  size = read(STDIN, buf, size-1);

  for (i = 0; i < size; i++) {
    if (*(buf+i) == '\n' || *(buf+i) == '\r') {
      *(buf+i) = '\0';
      break;
    }
  }

  *(buf+size) = '\0';
  return MIN(size, i);
}
