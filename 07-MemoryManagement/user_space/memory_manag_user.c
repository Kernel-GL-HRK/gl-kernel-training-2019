// SPDX-License-Identifier: GPL
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

typedef void *(*memory_allocate) (ssize_t size, void *context);
typedef void (*memory_free) (const void *mem, void *context);
typedef size_t (*memory_incr_size)(const size_t cur_size);

struct cnf_mem_allocate {
	memory_allocate mem_allocate;
	void *cnt;
};

struct cnf_mem_free {
	memory_free mem_free;
	void *cnt;
};

struct mem_config {
	struct cnf_mem_allocate cnf_allocate;
	struct cnf_mem_free	cnf_free;
	memory_incr_size incr_size;
	size_t start_size;
	size_t max_size_mem;

};

struct mem_description {
	struct mem_config config;
	char *buf;
	size_t size_buf;
};

static long long get_timeout(struct timespec *start, struct timespec *end)
{
	return 1000000000 * (end->tv_sec - start->tv_sec)
		+ (end->tv_nsec - start->tv_nsec);
}

static void mem_test(struct mem_description *descr)
{
	struct timespec ts_start, ts_end;

	long long time_spent_allocate;
	long long time_spent_free;

	size_t size = descr->config.start_size;
	int32_t numb_sample = 0;
	void *res_allocate;

	sprintf(descr->buf + strlen(descr->buf),
		"number\taddr\t\tsize\t\tallocate(ns)\tfree(ns)\n");

	while (size < descr->config.max_size_mem) {
		numb_sample++;

		clock_gettime(CLOCK_REALTIME, &ts_start);
		res_allocate = descr->config.cnf_allocate.mem_allocate(size,
				descr->config.cnf_allocate.cnt);
		clock_gettime(CLOCK_REALTIME, &ts_end);
		time_spent_allocate = get_timeout(&ts_start, &ts_end);

		if (res_allocate != NULL &&
			descr->config.cnf_free.mem_free != NULL) {
			clock_gettime(CLOCK_REALTIME, &ts_start);
			descr->config.cnf_free.mem_free(res_allocate,
					descr->config.cnf_free.cnt);
			clock_gettime(CLOCK_REALTIME, &ts_end);
			time_spent_free = get_timeout(&ts_start, &ts_end);
		} else {
			time_spent_free = 0;
		}

		sprintf(descr->buf + strlen(descr->buf),
					"%2u 	%10p   %10lu 	%10llu 	 %10llu\n",
			numb_sample, res_allocate, size,
			time_spent_allocate, time_spent_free);
		size = descr->config.incr_size(size);
	}
}

static size_t incr_shift_bit(size_t cur_size)
{
	return (cur_size << 1);
}

static void *mem_malloc(ssize_t size, void *context)
{
	return malloc(size);
}

static void mem_free(const void *mem, void *context)
{
	free((void *)mem);
}

static void *mem_calloc(ssize_t size, void *context)
{
	return calloc(1, size);
}

static void *mem_alloca(ssize_t size, void *context)
{
	return alloca(size);
}

int main(void)
{
	char buf[4000] = {0};
	struct mem_description descr = {0};

	descr.config.cnf_allocate.mem_allocate = mem_malloc;
	descr.config.cnf_free.mem_free = mem_free;
	descr.config.incr_size = incr_shift_bit;

	descr.buf = buf;
	descr.config.max_size_mem = 0xF00000000;
	descr.config.start_size = 1;
	sprintf(descr.buf + strlen(descr.buf), "malloc:\n");
	mem_test(&descr);
	printf("%s\n", buf);

	memset(buf, 0, sizeof(buf));

	sprintf(descr.buf + strlen(descr.buf), "calloc:\n");
	descr.config.cnf_allocate.mem_allocate = mem_calloc;
	mem_test(&descr);
	printf("%s\n", buf);

	memset(buf, 0, sizeof(buf));

	sprintf(descr.buf + strlen(descr.buf), "alloca:\n");
	descr.config.cnf_allocate.mem_allocate = mem_alloca;
	descr.config.cnf_free.mem_free = NULL;
	descr.config.max_size_mem = 0x100000;
	mem_test(&descr);
	printf("%s\n", buf);

	return 0;
}
