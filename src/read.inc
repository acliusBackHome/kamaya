%macro read 0
  section.text
    call READ
    mov [esp], eax
%endmacro

extern READ