CC := gcc
AR := ar
ASM := nasm
LD := ld

run: libfoo print.nasm macro.inc
	$(ASM) -f elf32 -P"macro.inc" -o print.o print.nasm
	$(LD) -m elf_i386 -o print.bin print.o libfoo.a
	echo "run ./print.bin to watch the result"

footest: libfoo
	$(CC) -m32 -S foo_test.c
	$(CC) -m32 -c foo_test.c
	$(LD) -m elf_i386 -o foo_test.bin foo_test.o libfoo.a
	echo "run ./foo_test.bin to watch the result"

libfoo: clean
	$(CC) -m32 -fno-stack-protector -c -o foo.o foo.c
	$(AR) -crv libfoo.a foo.o

clean:
	-rm *.o *.a *.bin