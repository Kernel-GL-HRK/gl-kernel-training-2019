#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[])
{
	time_t rawt;
	struct tm* abs_time;
	time(&rawt);
	abs_time = localtime(&rawt);
	printf("Current local time: %s", asctime(abs_time));
	return 0;
}
