#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATION 64

struct timespec time_spec1, time_spec2, result;
void *pt;

int main(void)
{
	//malloc
	printf("malloc\n");
	printf("ineration, size(hex), time\n");
	for (int i = 0; i <= ITERATION*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		timespec_get(&time_spec1, TIME_UTC);
		pt = malloc(size);
		timespec_get(&time_spec2, TIME_UTC);
		if (pt == NULL) {
			printf("pt = NULL\n");
			break;
		}
		free(pt);
		result.tv_sec = time_spec2.tv_sec - time_spec1.tv_sec;
		result.tv_nsec = time_spec2.tv_nsec - time_spec1.tv_nsec;
		if (result.tv_nsec < 0) {
			result.tv_nsec += 1000000000;
			result.tv_sec -= 1;
		}
		printf("%i, 0x%zx, %ld.%09ld\n",
				i/2, size, result.tv_sec, result.tv_nsec);
	}


	//calloc
	printf("calloc\n");
	printf("ineration, size(hex), time\n");
	for (int i = 0; i <= ITERATION*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		timespec_get(&time_spec1, TIME_UTC);
		pt = calloc(1, size);
		timespec_get(&time_spec2, TIME_UTC);
		if (pt == NULL) {
			printf("pt = NULL\n");
			break;
		}
		free(pt);
		result.tv_sec = time_spec2.tv_sec - time_spec1.tv_sec;
		result.tv_nsec = time_spec2.tv_nsec - time_spec1.tv_nsec;
		if (result.tv_nsec < 0) {
			result.tv_nsec += 1000000000;
			result.tv_sec -= 1;
		}
		printf("%i, 0x%zx, %ld.%09ld\n",
				i/2, size, result.tv_sec, result.tv_nsec);
	}

	//alloca
	printf("alloca\n");
	printf("ineration, size(hex), time\n");
	for (int i = 0; i <= ITERATION*2+1; i++) {
		size_t size = (1 << i/2);

		if (i%2 != 0)
			size += 1;

		timespec_get(&time_spec1, TIME_UTC);
		pt = alloca(size);
		timespec_get(&time_spec2, TIME_UTC);
		if (pt == NULL) {
			printf("pt = NULL\n");
			break;
		}

		result.tv_sec = time_spec2.tv_sec - time_spec1.tv_sec;
		result.tv_nsec = time_spec2.tv_nsec - time_spec1.tv_nsec;
		if (result.tv_nsec < 0) {
			result.tv_nsec += 1000000000;
			result.tv_sec -= 1;
		}
		printf("%i, 0x%zx, %ld.%09ld\n",
				i/2, size, result.tv_sec, result.tv_nsec);
	}

	return 0;
}
