#include <types.h>
#include <param.h>
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

int str2wstr(wchar *dst, const char *src, int srclen) {
  int i;
  for (i = 0; i < srclen; i++) {
    dst[i] = src[i];
  }
  dst[i] = 0;
  return i;
}

int wstr2str(char *dst, const char *src, int srclen) {
  int i;
  for (i = 0; i < srclen / 2; i++) {
    dst[i] = src[i * 2];
  }
  return i;
}
// 读目录项的字符串结尾自带一个0，所以写入的0xff是读不到的。
int fill_fat32_long_name(char *dst, const char *src) {
  int i;
  int j = 0;
  for (i = 0; i < src[i]; i++) {
    if (src[i] == 0xff)
      continue;
    dst[j] = src[i];
    j++;
  }
  dst[j] = '\0';
  return j;
}

int read_fat32_short_name(char *dst, const char *src) {
  int i;
  int j = 0;
  for (i = 0; i < 8; i++) {
    if (src[i] == FATDIR_DLT)
      break;
    dst[j] = src[i];
    j++;
  }
  for (int i = 8; i < 11; i++) {
    if (src[i] == FATDIR_DLT)
      break;
    if (i == 8) {
      dst[j] = '.';
      j++;
    }
    dst[j] = src[i];
    j++;
  }
  dst[j] = '\0';
  return j;
}

int write_fat32_short_name(char *dst, const char *src) {
  int i;
  int name, attr;
  for (name = 0; src[name] != '.' && src[name] != '\0'; name++)
    ;
  for (attr = name + 1; src[attr] != '\0'; attr++)
    ;
  attr = attr - name - 1;
  // if(name > 8 || attr > 3)
  //   return -1;

  for (i = 0; i < 8; i++) {
    if (i < name)
      dst[i] = src[i];
    else
      dst[i] = FATDIR_DLT;
  }
  for (int i = 8; i < 11; i++) {
    if (i - 8 < attr)
      dst[i] = src[name + 1 + (i - 8)];
    else
      dst[i] = FATDIR_DLT;
  }
  return 0;
}

