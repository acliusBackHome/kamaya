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
	./dist/bin/main < test/1.c > test/1.out
	./dist/bin/main < test/2.c > test/2.out
	./dist/bin/main < test/3.c > test/3.out

run: main