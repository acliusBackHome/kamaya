run: 0x1.l 0x1.y 0x1.1.in 0x1.2.in
	make clean
	bison -vdty -o 0x1.tab.cpp 0x1.y
	flex -o 0x1.yy.cpp 0x1.l
	g++ -o 0x1.bin 0x1.tab.cpp 0x1.yy.cpp 0x1.cpp
	@echo =============test start=============
	-./0x1.bin < 0x1.1.in
	-./0x1.bin < 0x1.2.in
	@echo =============test  over=============
clean:
	clear
	-rm *.tab.h *.tab.hpp *.yy.cpp *.tab.cpp *.output *.bin