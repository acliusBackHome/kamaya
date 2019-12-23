all: run

CC := g++

dist/scanner.yy.cpp: src/scanner.l dist/parser.tab.cpp
	flex -o $@ $<

dist/parser.tab.cpp: src/parser.y
	bison -vdty -o $@ $<

copy:
	cp src/*.hpp src/*.cpp dist/
	cp src/*.a dist/
	cp src/*.inc dist/

dist/kamaya.cpp: copy

compile: dist/parser.tab.cpp dist/scanner.yy.cpp dist/kamaya.cpp dist/ParseTree.cpp dist/ParseNode.cpp dist/ParseType.cpp dist/ParseExpression.cpp dist/ParseDeclaration.cpp dist/ParseTreeErrorHandle.cpp dist/ParseException.cpp dist/ParseExpressionConstCalculate.cpp dist/BaseBlock.cpp dist/x86.cpp dist/IR.cpp
	$(CC) -o dist/bin/main $^ -std=c++11

main:
	make clean
	make compile

clean:
	clear
	-mv dist/*.output dist/*.cpp dist/*.hpp test/*.out test/*.o test/*.out test/*.bin test/*.asm trash

test: main
	make only_test

only_test:
	for dir in $(shell ls test/*.c);\
		do \
			./dist/bin/main $$dir $$dir.asm > $$dir.out && \
			nasm -f elf32 -P"./dist/print.inc" -P"./dist/read.inc"  -o $$dir.o $$dir.asm && \
			ld -m elf_i386 -o $$dir.bin $$dir.o dist/libprint.a dist/libread.a  && \
			echo "\033[33m[ok]\033[0m \033[32m$$dir\033[0m built successfully. run ./$${dir}.bin to watch result" || \
			echo "\033[31m[no]\033[0m \033[32m$$dir\033[0m"; \
		done

run: main
	make test
