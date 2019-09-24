all: run

scanner.yy.cpp: scanner.l parser.tab.cpp
	flex -o $@ $<

parser.tab.cpp: parser.y
	bison -vdty -o $@ $<

main: parser.tab.cpp scanner.yy.cpp
	g++ -o $@ $^

clean:
	clear
	mv *.tab.* *.yy.* *.output crash

test: main
	./main < test.cpp

run: clean main