#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <sys/resource.h>

#define SEC_TO_NS	1e9

struct timespec ts;
long before, relalloc, relfree;
const char *strtypem[3] = {
	"MALLOC",
	"CALLOC",
	"ALLOCA"
};

int main(void)
{
	int i, j, ctype, *p;
	long allocsize;

	for (ctype = 0; ctype < 3; ctype++) {
		printf("-----------------------------------------------------------\n");
		printf("Type mem: %s\n", strtypem[ctype]);
		printf("2^X\tBuffer size(bytes)\tAllocation time(nS)\tFreeing time(nS)\n");
		for (i = 0, j = 0; i < 64; j++) {
			if (j == 2) {
				i++;
				j = 0;
			}
			printf("2^%d+%d", i, j);
			allocsize = pow(2, i) + j;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			before = ts.tv_sec * (int)SEC_TO_NS + ts.tv_nsec;
			switch (ctype) {
				case 0: {
					p = malloc(allocsize);
					break;
				}
				case 1: {
					p = calloc(allocsize, sizeof(int));
					break;
				}
				case 2: {
					struct rlimit rl = {};
					const rlim_t kStackSize = 1024L;
					int res = 0;

					res = getrlimit(RLIMIT_STACK, &rl);
					if (res != 0) {
						p = NULL;
						break;
					}
					rl.rlim_cur = kStackSize*32;// 32kB
					rl.rlim_max = kStackSize*8*1024;// 8MB
					res = setrlimit(RLIMIT_STACK, &rl);
					if (res == 0 &&
						allocsize <= (long)rl.rlim_cur)
						p = alloca(allocsize);
					else
						p = NULL;
					break;
				}
			}
			if (p == NULL) {
				printf("%15ld\t\t", allocsize);
				printf("Mem %s error\n", strtypem[ctype]);
				break;
			}
			clock_gettime(CLOCK_MONOTONIC, &ts);
			relalloc = ts.tv_sec * (int)SEC_TO_NS +
					ts.tv_nsec - before;
			before = ts.tv_sec * (int)SEC_TO_NS + ts.tv_nsec;
			printf("%15ld\t\t", allocsize);
			if (ctype != 2) {
				free(p);
				clock_gettime(CLOCK_MONOTONIC, &ts);
				relfree = ts.tv_sec * (int)SEC_TO_NS +
						ts.tv_nsec - before;
				printf("%06ld\t\t\t%06ld\n", relalloc, relfree);
			} else {
				printf("%06ld\t\t\t---\n", relalloc);
			}
		}
	}
	return EXIT_SUCCESS;
}
