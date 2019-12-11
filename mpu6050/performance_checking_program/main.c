#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int check_perfomance(const char *sysfs_file_name, time_struct *ptr_data)
{
	unsigned long long time_ns;
	int fd = -1;
	int cnt = 0;
	char buf[BUF_SIZE] = {0};

	fd = open(sysfs_file_name, O_RDONLY);

		if (fd < 0) {
			printf("Error while open file!\n");
			return 1;
		}

	clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_start);
	cnt = read(fd, buf, sizeof(buf));

		if (cnt != 0) {
			clock_gettime(CLOCK_MONOTONIC, &ptr_data->time_end);
			time_ns = time_interval(&ptr_data->time_end,
					&ptr_data->time_start);
			printf("%s\r\x1b[48C%llu\r\x1b[96C%llu\n", sysfs_file_name,
					time_ns, (NS / time_ns));
			close(fd);
			return 0;
		} else {
			printf("Error while reading file!\n");
			close(fd);
			return 1;
		}
}

int main(int argc, char *argv[])
{
	printf("file_name:\r\x1b[48Creading_time, ns\r\x1b[96Ccnt_per_sec\n");

	check_perfomance(ACCEL_X_FILE, &time_data);
	check_perfomance(ACCEL_Y_FILE, &time_data);
	check_perfomance(ACCEL_Z_FILE, &time_data);
	check_perfomance(GYRO_X_FILE, &time_data);
	check_perfomance(GYRO_Y_FILE, &time_data);
	check_perfomance(GYRO_Z_FILE, &time_data);
	check_perfomance(TEMP_FILE, &time_data);

	exit(0);
}
