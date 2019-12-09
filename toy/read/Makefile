CC := gcc
AR := ar
ASM := nasm

read_asm_test: libread macro.inc read_asm_test.nasm
	$(ASM) -f elf32 -P"macro.inc" -o read_asm_test.o read_asm_test.nasm
	$(LD) -m elf_i386 -o read_asm_test.bin read_asm_test.o libread.a
	echo "run ./read_asm_test.bin to watch result"

libread: clean
	$(CC) -m32 -fno-stack-protector -c -o read.o read.c
	$(AR) -crv libread.a read.o

clean:
	-rm *.o *.a
