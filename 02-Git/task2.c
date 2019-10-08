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

	if(in == out)
		printf("> Our choice is the same\n> Draw\n");
	else if(in == 'r' && out == 'p')
                printf("> You choose rock, I choose paper\n> I win: paper beats rock\n");
	else if(in == 'r' && out == 's')
                printf("> You choose rock, I choose scissors\n> You win: rock beats scissors\n");
	else if(in == 'p' && out == 'r')
                printf("> You choose paper, I choose rock\n> You win: paper beats rock\n");
	else if(in == 'p' && out == 's')
                printf("> You choose paper, I choose scissors\n> I win: scissors cuts paper\n");
	else if(in == 's' && out == 'r')
                printf("> You choose scissors, I choose rock\n> I win: rock beats scissors\n");
	else if(in == 's' && out == 'p')
                printf("> You choose scissors, I choose paper\n> You win: scissors cuts paper\n");
	else printf("> Your choice is wrong\n");

	return 0;
}
