CC := gcc
AR := ar
ASM := nasm
LD := ld

test_print: test_print.nasm print.inc libprint
	$(ASM) -f elf32 -P"print.inc" -o test_print.o test_print.nasm
	$(LD) -m elf_i386 -o test_print.bin test_print.o libprint.a
	echo "run ./test_print.bin to watch the result"

libprint: clean
	$(CC) -m32 -fno-stack-protector -c -o print.o print.c
	$(AR) -crv libprint.a print.o

clean:
	-rm *.o *.a *.bin