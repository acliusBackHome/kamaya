0x1: 0x1.l 0x1.y 0x1.in
	make clean
	bison -vdty -o 0x1.tab.cpp 0x1.y
	flex -o 0x1.yy.cpp 0x1.l
	g++ -o 0x1.bin 0x1.tab.cpp 0x1.yy.cpp 0x1.cpp
	@echo =============test start=============
	./0x1.bin < 0x1.in
	@echo =============test  over=============
0x2:
	-rm 0x2.bin
	g++ -o 0x2.bin 0x2.cpp
	./0x2.bin
C98:
	bison -vdty -o C98.tab.cpp C98.y
	flex -o C98.yy.cpp C98.l
	g++ -o C98.bin C98.tab.cpp C98.yy.cpp
	-rm *.tab.h *.tab.hpp *.output
clean:
	clear
	-rm *.tab.h *.tab.hpp *.yy.cpp *.tab.cpp *.output *.bin