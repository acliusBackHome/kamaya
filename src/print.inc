%macro print 1
  ; 宏有 1 个参数
  section .data
    %%string db  %1, 10, 0 ; 添加 \n \0
  section .text
    push dword %%string
    call PRINT ; 呼叫联编进来的 print 函数
    shl eax, 2 ; eax 左移两位
    add esp, eax ; esp += eax
%endmacro

extern PRINT

