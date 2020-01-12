#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_CYCLES 100
#define FILE_NAME "/sys/class/mpu6050/temperature"

void *perf_test(void *ptr)
{
	struct timespec tstart, tend;
	int do_test = NUM_CYCLES;
	double diff;
	int fdis, numread;
	char buff[64];

	while (--do_test) {
		fdis = open(FILE_NAME, O_RDONLY);
		if (fdis < 0) {
			printf("Open sys file error\n");
			break;
		}
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
		numread = read(fdis, buff, sizeof(buff) - 1);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);
		diff = (tend.tv_sec - tstart.tv_sec) * 1e6 +
			(tend.tv_nsec - tstart.tv_nsec) / 1e3;
		printf("%d: %.3f msec temp: %s\n", do_test, diff, buff);
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	pthread_t perf_thread;
	int ret;

	ret = pthread_create(&perf_thread, NULL, perf_test, NULL);
	pthread_join(perf_thread, NULL);
	return 0;
}
