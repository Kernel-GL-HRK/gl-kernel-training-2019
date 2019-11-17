/*
 * main.c
 *
 *  Created on: 17 нояб. 2019 г.
 *      Author: bekir
 */
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <malloc.h>
#include "main.h"

time_struct time_data = DEFAULT_TIME_STRUCT_INIT;

unsigned long long time_interval(struct timespec *time_end,
		struct timespec *time_start)
{
	unsigned long long time_buf = 0;

	time_buf = (unsigned long long)(time_end->tv_sec) * NS +
			time_end->tv_nsec;
	time_buf -= ((unsigned long long)(time_start->tv_sec) * NS +
			time_start->tv_nsec);

	return time_buf;
}

void malloc_time_calc(time_struct *ptr_data)
{
	char *malloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			malloc_ptr = malloc(1 << order);
				if (malloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", 1 << order);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			free(malloc_ptr);
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

void malloc_time_calc_odd_size(time_struct *ptr_data)
{
	char *malloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			malloc_ptr = malloc((1 << order) + 1);
				if (malloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str +
			strlen(ptr_data->buf_str), "%u", (1 << order) + 1);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			free(malloc_ptr);
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

void calloc_time_calc(time_struct *ptr_data)
{
	char *calloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			calloc_ptr = calloc(1 << order, sizeof(char));
				if (calloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str +
			strlen(ptr_data->buf_str), "%u", 1 << order);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			free(calloc_ptr);
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

void calloc_time_calc_odd_size(time_struct *ptr_data)
{
	char *calloc_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\r\x1b[48Cfreeing time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			calloc_ptr = calloc(((1 << order) + 1), sizeof(char));
				if (calloc_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str +
			strlen(ptr_data->buf_str), "%u", (1 << order) + 1);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu", time_ns);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			free(calloc_ptr);
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[48C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

static void *alloca_test(size_t size)
{
	return alloca(size);
}

void alloca_time_calc(time_struct *ptr_data)
{
	char *alloca_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			alloca_ptr = (char *)alloca_test(1 << order);
				if (alloca_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", 1 << order);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

void alloca_time_calc_odd_size(time_struct *ptr_data)
{
	char *alloca_ptr;
	unsigned long long time_ns;
	size_t order = 0;

	sprintf(ptr_data->buf_str,
	"size:\r\x1b[24Callocation time, ns:\n");

		for (order = 1; order <= 31; order++) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
			alloca_ptr = (char *)alloca_test((1ULL << order) + 1);
				if (alloca_ptr == NULL) {
					sprintf(ptr_data->buf_str +
					strlen(ptr_data->buf_str), "\n");
					break;
				}
			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
				"%u", (1 << order) + 1);

			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);

			sprintf(ptr_data->buf_str + strlen(ptr_data->buf_str),
					"\r\x1b[24C%llu\n", time_ns);
		}
		printf("%s", ptr_data->buf_str);
}

int main(int argc, char *argv[])
{
	printf("\x1b[31m\nMALLOC_TEST\n\x1b[0m");
	malloc_time_calc(&time_data);
	printf("\x1b[31m\nMALLOC_TEST_ODD_SIZE\n\x1b[0m");
	malloc_time_calc_odd_size(&time_data);
	printf("\x1b[31m\nCALLOC_TEST\n\x1b[0m");
	calloc_time_calc(&time_data);
	printf("\x1b[31m\nCALLOC_TEST_ODD_SIZE\n\x1b[0m");
	calloc_time_calc_odd_size(&time_data);
	printf("\x1b[31m\nALLOCA_TEST\n\x1b[0m");
	alloca_time_calc(&time_data);
	printf("\x1b[31m\nALLOCA_TEST_ODD_SIZE\n\x1b[0m");
	alloca_time_calc_odd_size(&time_data);

	exit(0);
}

