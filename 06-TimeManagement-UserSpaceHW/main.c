#include <stdio.h>
#include <time.h>

int  main(int argc, char *argv[])
{
	time_t t = time(NULL);
	struct tm *aTm = localtime(&t);
	printf(asctime(aTm));
	return 0;
}
