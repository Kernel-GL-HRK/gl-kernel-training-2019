#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#define ATTRS	7
#define SEC_TO_NS	1e9

const char *input_attr[ATTRS] = {
	"/sys/class/mpu6050/accel_x",
	"/sys/class/mpu6050/accel_y",
	"/sys/class/mpu6050/accel_z",
	"/sys/class/mpu6050/gyro_x",
	"/sys/class/mpu6050/gyro_y",
	"/sys/class/mpu6050/gyro_z",
	"/sys/class/mpu6050/temp"
};

long get_timestamp(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * (int)SEC_TO_NS + ts.tv_nsec;
}

void read_file(const char *path)
{
	int fd = 0, ret = 0;
	char str[32] = {0};

	fd = open(path, O_RDONLY);

	if (fd < 0) {
		fprintf(stderr, "Error opening %s for reading\n", path);
		exit(EXIT_FAILURE);
	}

	ret = read(fd, str, sizeof(str));
	if (ret < 0) {
		fprintf(stderr, "Read error %s\n", path);
		exit(EXIT_FAILURE);
	}

	printf("%s: %s", path, str);

	close(fd);
}

int main(void)
{
	u_int8_t fil = 0;
	long t1, t2;

	while (1) {
		printf("Reading...\n");
		t1 = get_timestamp();
		for (fil = 0; fil < ATTRS; fil++)
			read_file(input_attr[fil]);
		t2 = get_timestamp();
		printf("Time readed nS = %06ld\n", t2 - t1);

		sleep(1);
	}
}
