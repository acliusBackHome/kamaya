run:
	g++ -Wall -O2 -fPIC -I./ -c lika.c
	ar cvr -o liblika.a lika.o
	g++ lika_test.c -o lika_test.bin ./liblika.a
	@echo "test 1 + 1 = "
	@echo "run ./lika_test.bin || echo [dollar]? to watch result"