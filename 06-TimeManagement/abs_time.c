// SPDX-License-Identifier: GPL
#include <stdio.h>
#include <time.h>

int main(void)
{
	time_t sec;

	time(&sec);
	printf("Date: %s", ctime(&sec));

	return 0;
}
