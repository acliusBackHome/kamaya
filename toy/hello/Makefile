run:
	-rm *.o *.a *.bin
	nasm -f elf32 -o hello.o hello.nasm
	ld -m elf_i386 -o hello.bin hello.o
	echo "run ./hello.bin to watch result"