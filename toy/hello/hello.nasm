
global _start
[section .text]
_start:
  mov eax, 4
  mov ebx, 1
  mov ecx, msg
  mov edx, len
  int 0x80
  mov eax, 1
  mov ebx, 0
  int 0x80

[section .data]
  msg: db "hello world!", 10
  len: equ $-msg