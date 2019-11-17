
memory_test: main.o
	gcc -o memory_test main.o -lc

main.o: main.c
	gcc -c main.c -o main.o

.PHONY: clean

clean:
	-rm *.o memory_test
