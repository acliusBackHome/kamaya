all: run

dist/scanner.yy.cpp: src/scanner.l dist/parser.tab.cpp
	flex -o $@ $<

dist/parser.tab.cpp: src/parser.y
	bison -vdty -o $@ $<

dist/kamaya.cpp:
	cp src/kamaya.hpp src/kamaya.cpp src/MessageTree.hpp dist/

main: dist/parser.tab.cpp dist/scanner.yy.cpp dist/kamaya.cpp
	g++ -o dist/bin/$@ $^

clean:
	clear
	-mv dist/*.output dist/*.cpp dist/*.hpp test/*.out trash

test:
	for dir in $(shell ls test/*.c);\
		do \
			./dist/bin/main < $$dir > $$dir.out; \
		done

run: main
