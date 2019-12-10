# libread

libread 用于读取一个整数

* 使用

在汇编文件中使用 read 语句

```nasm
read
```

read 所得整数存储在 eax 中

* 编译

需要 libread.a, read.inc 两个文件

```
nasm -f elf32 -P"read.inc" -o test_read.o test_read.nasm # read.inc 在这里使用
ld -m elf_i386 -o test_read.bin test_read.o libread.a # libread.a 在这里使用
```

* 编译 libread.a

需要 read.c read.h

```
make libread
```

或者

```
g++ -m32 -fno-stack-protector -c -o read.o read.c
ar -crv libread.a read.o
```

生成的 libread.a 可以和 .o 链接