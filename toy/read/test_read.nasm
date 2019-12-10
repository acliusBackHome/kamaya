global _start
section .text
_start:
  read ; 呼叫宏
  push eax ; 存储结果
  mov eax, 4
  mov ebx, 1
  mov ecx, msg
  mov edx, len
  int 0x80
  pop eax
  mov ebx, eax
  mov eax, 1
  int 0x80
section .data
  msg: db 'run echo $? to watch result', 10, 0
  len: equ $-msg