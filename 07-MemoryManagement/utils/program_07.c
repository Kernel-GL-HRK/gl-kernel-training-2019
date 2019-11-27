// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  program_06.c - Test time
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define MAX_POW 32
#define INTERVAL_1NS 1000000000ULL
#define POW2(x) (2UL << (size_t)(x))
#define COUNT_STAT_POINT 6

#define ADD_MEM 1

enum STAT_NAME {
	NAME_MALLOC = 0,
	NAME_FREE_MALLOC,
	NAME_CALLOC,
	NAME_FREE_CALLOC,
	NAME_ALLOCA,
	NAME_NULL
};

static char *statistics_name[] = {
	"malloc",
	"free",
	"calloc",
	"free",
	"alloca",
	"NULL"
};

struct statistic_info {
	unsigned long long dt[COUNT_STAT_POINT];
};

static struct statistic_info statistics[2*MAX_POW];


unsigned long long calck_time(const struct timespec *begin,
	const struct timespec *end);
void *fun_malloc(size_t num, size_t size);
void *fun_alloca(size_t num, size_t size);
void *fun_calloc(size_t num, size_t size);

inline unsigned long long calck_time(const struct timespec *begin,
	const struct timespec *end)
{
	return (end->tv_sec * INTERVAL_1NS + end->tv_nsec) -
			(begin->tv_sec * INTERVAL_1NS + begin->tv_nsec);
}


inline void *fun_malloc(size_t num, size_t size)
{
	return malloc(size);
}

inline void *fun_alloca(size_t num, size_t size)
{
	return alloca(size);
}

inline void *fun_calloc(size_t num, size_t size)
{
	return calloc(num, size);
}

static void test_mem(
	void * (*fun_alloc)(size_t, size_t),
	void (*fun_free)(void *),
	enum STAT_NAME name_alloc,
	enum STAT_NAME name_free
	)
{

	unsigned long long i, j;
	char *buf;
	struct timespec tim_begin, tim_end;

	size_t pow = 0;

	for (i = 0; i <= 2*MAX_POW; i += 2) {
		for (j = 0; j < 2; ++j) {
			size_t new_size = POW2(pow)+j*ADD_MEM;

			timespec_get(&tim_begin, TIME_UTC);
			buf = fun_alloc(1, new_size);
			timespec_get(&tim_end, TIME_UTC);

			if (buf == NULL) {
				printf("%s: error\n",  __func__);
				return;
			}

			statistics[i+j].dt[name_alloc] =
				calck_time(&tim_begin, &tim_end);

			if (name_alloc != NAME_ALLOCA) {

				timespec_get(&tim_begin, TIME_UTC);
				free(buf);
				timespec_get(&tim_end, TIME_UTC);

				statistics[i+j].dt[name_free] =
					calck_time(&tim_begin, &tim_end);
			} else {
				statistics[i+j].dt[name_free] = 0;
			}
		}

		pow++;
		usleep(100);
	}
}

void Print_Help(void)
{
	printf("Using: program_07 [count of test]\nDefault count of test is 1");
}

int main(int argc, char *argv[])
{

	int i, j, t, num_test;

	num_test = 1;

	if (argc == 2) {
		num_test = atoi(argv[1]);
		if (!num_test) {
			Print_Help();
			num_test = 1;
		}
	}

	printf(
		"%-12s%-12s%-12s%-12s%-12s%-12s\n",
		"size/dt",
		statistics_name[NAME_MALLOC],
		statistics_name[NAME_FREE_MALLOC],
		statistics_name[NAME_CALLOC],
		statistics_name[NAME_FREE_CALLOC],
		statistics_name[NAME_ALLOCA]
		);

	for (t = 0; t <  num_test; ++t) {
		test_mem(fun_malloc, free, NAME_MALLOC, NAME_FREE_MALLOC);
		test_mem(fun_calloc, free, NAME_CALLOC, NAME_FREE_CALLOC);
		test_mem(fun_alloca, free, NAME_ALLOCA, NAME_NULL);

		size_t pow = 0;

		for (i = 0; i <= 2*MAX_POW; i += 2) {
			for (j = 0; j < 2; ++j) {

				printf(
					"%-12lu%-12llu%-12llu%-12llu%-12llu%-12llu\n",
					POW2(pow)+j*ADD_MEM,
					statistics[i+j].dt[NAME_MALLOC],
					statistics[i+j].dt[NAME_FREE_MALLOC],
					statistics[i+j].dt[NAME_CALLOC],
					statistics[i+j].dt[NAME_FREE_CALLOC],
					statistics[i+j].dt[NAME_ALLOCA]
					);
			}
			pow++;
		}
	}
	return 0;
}
