run: clean foo.c print.nasm macro.inc
	gcc -m32 -c -o foo.o foo.c
	ar -crv libfoo.a foo.o
	nasm -f elf32 -P"macro.inc" -o print.o print.nasm
	ld -m elf_i386 -o print.bin print.o -L. -lfoo
	echo "run ./print.bin to watch the result"

hello: clean
	-rm hello hello.o
	nasm -f elf32 -o hello.o hello.nasm
	ld -m elf_i386 -o hello.bin hello.o
	echo "run ./hello.bin to watch result"

clean:
	-rm *.o *.a