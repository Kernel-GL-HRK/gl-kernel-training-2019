dummy.exe:	Dummy_Program.o
	gcc -o dummy.exe Dummy_Program.o
	gcc -o Game.exe Game.o
 
Dummy_Program.o:	Dummy_Program.c
	gcc -O0 -g3 -Wall -c Dummy_Program.c
	gcc -masm=intel -g3 -Wall -c Dummy_Program.c -S
	
Game.o:	Game.c
	gcc -O0 -g3 -Wall -c Game.c
	gcc -masm=intel -g3 -Wall -c Game.c -S
