void x(char *buf, int len) {
  __asm__(
    ".intel_syntax noprefix\n\
       push ebx\n\
       push ecx\n\
       push edx\n\
       mov eax, 4\n\
       mov ebx, 1\n\
       mov ecx, [ebp+4*2]\n\
       mov edx, 1\n\
       int 0x80\n\
       mov eax, 3\n\
       mov ebx, 2\n\
       mov ecx, [ebp+4*2]\n\
       mov edx, 2\n\
       int 0x80\n\
       mov eax, 4\n\
       mov ebx, 1\n\
       mov ecx, [ebp+4*2]\n\
       mov edx, 2\n\
       int 0x80\n\
       pop edx\n\
       pop ecx\n\
       pop ebx\n\
       mov eax, 1\n\
       mov ebx, 2\n\
       int 0x80\n\
    .att_syntax"
  );
}
int _start() {
  char buf[1024];
  buf[0]='a';
  int len=1024;
  x(buf, len-1);
}