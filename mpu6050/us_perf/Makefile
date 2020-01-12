perf_test: main.o
	arm-linux-gnueabihf-gcc -o perf_test main.o -lpthread -lc

main.o: main.c
	arm-linux-gnueabihf-gcc -c main.c -o main.o

.PHONY: clean

clean:
	-rm *.o perf_test
