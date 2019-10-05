#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[])
{
	char in, out;

	printf("> Please choose: rock (r) - paper (p) - scissors (s)\n> ");
	in = getchar();
	srand(time(NULL));
	switch((rand() % 3) + 1)
	{
		case 1: out = 'r'; break;
		case 2: out = 'p'; break;
		case 3: out = 's'; break;
	}

	return 0;
}
