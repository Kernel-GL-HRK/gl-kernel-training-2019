// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  mpu6050_user.c - Test time
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <asm-generic/param.h>

#define NS_IN_SEC 1e9

long get_time(void)
{
	struct timespec ts;

	timespec_get(&ts, TIME_UTC);

	return ts.tv_sec * (int)NS_IN_SEC + ts.tv_nsec;
}

int main(int argc, char *argv[])
{
	long start_t, end_t;
	int fd = 0, count = 0;
	char line[255];
	size_t len;

	start_t = get_time();
	end_t = get_time();

	while ((end_t - start_t) < NS_IN_SEC) {
		fd = open("/sys/class/mpu6050/all", O_RDONLY);
		if (fd < 0) {
			printf("Error!\n");
			exit(EXIT_FAILURE);
		}

		len = read(fd, line, sizeof(line));
		close(fd);

		line[len] = 0;

		count++;
		end_t = get_time();
	}

	printf("Time :\t%09ld ns\nCount:\t%d\n", (end_t - start_t), count);

	return 0;
}
