0x1: 0x1.l 0x1.y 0x1.in
	bison -vdty -o 0x1.tab.cpp 0x1.y
	flex -o 0x1.yy.cpp 0x1.l
	g++ -o 0x1.bin 0x1.tab.cpp 0x1.yy.cpp
	-rm *.c *.cpp *.tab.h *.tab.hpp *.output
	@echo =============test start=============
	./0x1.bin < 0x1.in
	@echo =============test  over=============
0x2:
	-rm 0x2.bin
	g++ -o 0x2.bin 0x2.cpp
	./0x2.bin
clean:
	-rm *.c *.cpp *.tab.h *.tab.hpp *.output *.bin