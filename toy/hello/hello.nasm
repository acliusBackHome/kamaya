
global _start
[section .text]
_start:


  ; 1 + 1
  mov eax, 1
  mov ebx, 1
  add eax, ebx
  mov edx, eax

  mov eax, 4
  mov ebx, 1
  mov ecx, msg
  int 0x80

  mov eax, 1
  mov ebx, 0
  int 0x80

[section .data]
  msg: db "hello world!", 10
  len: equ $-msg
