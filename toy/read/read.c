#include "read.h"

#define BUFLEN 1024

int STRLEN(char *s);
int SYS_READ(char *buf, int len);

int READ() {
  char buf[BUFLEN], *p = buf, *p_end;
  int len = SYS_READ(buf, BUFLEN-1), value = 0, negative = 0;
  p_end = buf + len + 1;
  while (p != p_end) {
    if (*p == ' ' || *p == '\t') { // 跳过输入行开头的空白字符
      p++;
    } else {
      break;
    }
  }
  if (p != p_end && *p == '-') {
    negative = 1;
    p++;
  }
  while(p != p_end) {
    if ('0' <= *p && *p <= '9') {
      value = value * 10 + *p - '0';
      p++;
    } else {
      break;
    }
  }
  if (negative) {
    value = -value;
  }
  return value;
}

int STRLEN(char *s) {
  int i = 0;
  while (*s++) {
    i++;
  }
  return i;
}

int SYS_READ(char *buf, int len) {
  /*           eax ebx           ecx     edx
   * sys_read  3   unsigned int  char *  size_t
   * sys_read 的系统调用编号
   * 文件描述符 unsigned int fd
   * char __user * buf
   * size_t count
   */
  __asm__(
    ".intel_syntax noprefix\n\
       push ebx\n\
       push ecx\n\
       push edx\n\
       mov eax, 3\n\
       mov ebx, 2\n\
       mov ecx, [ebp+4*2]\n\
       mov edx, [ebp+4*3]\n\
       int 0x80\n\
       pop edx\n\
       pop ecx\n\
       pop ebx\n\
    .att_syntax"
  ); // 读入的数量存储在 eax（未弹出），值（char bytes）存储在 buf
}