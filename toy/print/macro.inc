%macro print 1
  ; 宏有 1 个参数
  [section .data]
    %%string db  %1, 10, 0
  [section .text]
    push dword %%string
    call PRINT ; 呼叫连编进来的 print 函数
    shl eax, 2 ; eax 左移两位
    add esp, eax ; esp += eax
%endmacro

%macro exit 1
  mov eax, 1
  mov ebx, %1 ; 第一个参数
  int 0x80 ; 触发系统调用
%endmacro

extern PRINT
global _start

[section .text]
_start:
