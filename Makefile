all: run

dist/scanner.yy.cpp: src/scanner.l dist/parser.tab.cpp
	flex -o $@ $<

dist/parser.tab.cpp: src/parser.y
	bison -vdty -o $@ $<

main: dist/parser.tab.cpp dist/scanner.yy.cpp
	g++ -o dist/bin/$@ $^

clean:
	clear
	mv dist/*.tab.* dist/*.yy.* dist/*.output test/*.out trash

test: main
	for dir in $(shell ls test/*.c);\
		do \
			./dist/bin/main < $$dir > $$dir.out; \
		done

run: main