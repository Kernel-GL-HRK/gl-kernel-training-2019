// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  program_06.c - Test time
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <time.h>
#include <stdio.h>
#include <asm-generic/param.h>

int main(int argc, char *argv[])
{
	struct timespec ts;
	char buff[100];

	timespec_get(&ts, TIME_UTC);
	strftime(buff, sizeof(buff), "%D %T", gmtime(&ts.tv_sec));
	printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);

	return 0;
}
