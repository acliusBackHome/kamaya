#include "foo.h"

void SYS_PRINT(char *string, int len);

#define BUFLEN 1024

int PRINT(char *fmt, ...) {
  int *args = (int*)&fmt;
  char buf[BUFLEN];
  char *p1 = fmt, *p2 = buf + BUFLEN;
  int len = -1, argc = 1;
  while (*p1++);
  do {
    p1--;
    if (*p1 == '%' && *(p1+1) == 'd') {
      p2++; len--; argc++;
      int num = *(++args), negative = 0;
      if (num < 0) {
        negative = 1;
        num = -num;
      }
      do {
        *(--p2) = num % 10 + '0';
        len++;
        num /= 10;
      } while (num);
      if (negative) {
        *(--p2) = '-';
        len++;
      }
    } else {
      *(--p2) = *p1;
      len++;
    }
  } while (p1 != fmt);
  SYS_PRINT(p2, len);
  return argc;
}

void SYS_PRINT(char *string, int len) {
  /* 内联汇编的 INTEL 语法 */
  /* 第一个参数位于 ebp+4*1 */
  /* 第二个参数位于 ebp+4*2 */
  /*            eax ebx           ecx           edx
   * sys_write  4   unsigned int  const char *  size_t
   */
  __asm__(
    ".intel_syntax noprefix\n\
       push eax\n\
       push ebx\n\
       push ecx\n\
       push edx\n\
       mov eax, 4\n\
       mov ebx, 1\n\
       mov ecx, [ebp+4*2]\n\
       mov edx, [ebp+4*3]\n\
       int 0x80\n\
       pop edx\n\
       pop ecx\n\
       pop ebx\n\
       pop eax\n\
     .att_syntax"
  );
}