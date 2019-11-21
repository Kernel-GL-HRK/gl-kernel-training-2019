#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <asm-generic/param.h>

int main(int argc, char *argv[])
{
	time_t time_variable = time(NULL);
	struct tm date_time = *localtime(&time_variable);

	printf("Current date: %02d/%02d/%04d\n", date_time.tm_mday,
		 date_time.tm_mon+1, date_time.tm_year+1900);
	printf("Current time: %02d:%02d:%02d\n", date_time.tm_hour,
		date_time.tm_min, date_time.tm_sec);
	struct timespec timespec_var;
	char buffer[200];

	timespec_get(&timespec_var, TIME_UTC);
	strftime(buffer, sizeof(buffer), "%D %T",
	 gmtime(&timespec_var.tv_sec));
	printf("Current time in UTC: %s.%09ld UTC\n", buffer,
	 timespec_var.tv_nsec);
	return 0;
}
