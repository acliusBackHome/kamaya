all: run

dist/scanner.yy.cpp: src/scanner.l dist/parser.tab.cpp
	flex -o $@ $<

dist/parser.tab.cpp: src/parser.y
	bison -vdty -o $@ $<

copy:
	cp src/*.hpp src/*.cpp dist/

dist/kamaya.cpp: copy

compile: dist/parser.tab.cpp dist/scanner.yy.cpp dist/kamaya.cpp
	g++ -o dist/bin/main $^

main:
	make clean
	make compile

clean:
	clear
	-mv dist/*.output dist/*.cpp dist/*.hpp test/*.out trash

test: main
	for dir in $(shell ls test/*.c);\
		do \
			./dist/bin/main < $$dir > $$dir.out; \
		done

run: main
