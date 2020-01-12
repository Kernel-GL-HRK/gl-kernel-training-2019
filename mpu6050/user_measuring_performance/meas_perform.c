#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define ATTR_AMOUNT 7
#define CLASS_PATH "/sys/class/mpu6050/"

char *sysAttrPath[] = {
	CLASS_PATH"accel_x",
	CLASS_PATH"accel_y",
	CLASS_PATH"accel_z",
	CLASS_PATH"gyro_x",
	CLASS_PATH"gyro_y",
	CLASS_PATH"gyro_z",
	CLASS_PATH"temp"
};

int measureReadFreq(int *counter)
{
	struct timespec tStart, tCurrent;
	*counter = 0;

	// get start measuring time
	clock_gettime(CLOCK_REALTIME, &tStart);

	do {
		for (int i = 0; i < ATTR_AMOUNT; i++) {
			int fd = 0, ret = 0;
			char str[32] = {0};

			fd = open(sysAttrPath[i], O_RDONLY);
			if (fd < 0)
				return 0;

			ret = read(fd, str, sizeof(str));
			if (ret < 0)
				return 0;

			close(fd);
		}

		(*counter)++;
		clock_gettime(CLOCK_REALTIME, &tCurrent);

	} while (tCurrent.tv_sec == tStart.tv_sec ||
				tCurrent.tv_nsec < tStart.tv_nsec);
}

int main(void)
{
	int counter;

	if (measureReadFreq(&counter))
		printf("Amount of readings per second: %d\n", counter);
	else
		printf("Error\n");

	return 0;
}
