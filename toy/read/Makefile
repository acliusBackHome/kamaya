CC := gcc
AR := ar
ASM := nasm

test_read: libread read.inc test_read.nasm
	$(ASM) -f elf32 -P"read.inc" -o test_read.o test_read.nasm
	$(LD) -m elf_i386 -o test_read.bin test_read.o libread.a
	echo "run ./test_read.bin to watch result"

libread: clean
	$(CC) -m32 -fno-stack-protector -c -o read.o read.c
	$(AR) -crv libread.a read.o

clean:
	-rm *.o *.a *.bin
