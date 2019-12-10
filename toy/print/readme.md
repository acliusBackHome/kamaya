# libprint

libprint 用于格式化输出包含整数的字符串

* 使用

在汇编文件中使用 print 语句

```nasm
push 141
print "I will out put format string with an integer 141: %d"
```

* 编译

需要 libprint.a, print.inc 两个文件

```
nasm -f elf32 -P"print.inc" -o test_print.o test_print.nasm # print.inc 在这里使用
ld -m elf_i386 -o test_print.bin test_print.o libprint.a # libprint.a 在这里使用
```

* 编译 libprint.a

需要 print.c print.h

```
make libprint
```

或者

```
g++ -m32 -fno-stack-protector -c -o print.o print.c
ar -crv libprint.a print.o
```

生成的 libprint.a 可以和 .o 链接