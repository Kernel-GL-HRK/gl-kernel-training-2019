hello.exe:	main.o
	gcc -o hello.exe main.o -lc

main.o:	main.c
	gcc -c main.c -o main.o

.PHONY : clean

clean :
	-rm *.o $(objects) 
