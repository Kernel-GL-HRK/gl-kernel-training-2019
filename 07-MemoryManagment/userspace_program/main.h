/*
 * main.h
 *
 *  Created on: 17 нояб. 2019 г.
 *      Author: bekir
 */
#ifndef MAIN_H_
#define MAIN_H_

#include <time.h>

#define NS 1000000000UL
#define BUF_SIZE 2048

typedef struct {
	struct timespec time_end;
	struct timespec time_start;
	char buf_str[BUF_SIZE];
} time_struct;

#define DEFAULT_TIME_STRUCT_INIT { {0, 0}, {0, 0}, {0} }

unsigned long long time_interval(struct timespec *time_end,
					struct timespec *time_start);
void malloc_time_calc(time_struct *ptr_data);
void malloc_time_calc_odd_size(time_struct *ptr_data);
void calloc_time_calc(time_struct *ptr_data);
void calloc_time_calc_odd_size(time_struct *ptr_data);
void alloca_time_calc(time_struct *ptr_data);
void alloca_time_calc_odd_size(time_struct *ptr_data);

#endif /* MAIN_H_ */
