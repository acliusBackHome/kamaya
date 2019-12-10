global _start
section .text
_start:
  push dword 1
  push dword 2
  push dword 3
  print "a = %d, b = %d, c = %d"
  push dword 1
  print "%d"
  mov eax, 1
  mov ebx, 0
  int 0x80