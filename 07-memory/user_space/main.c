#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NVALUE 64
#define NSTAT 10

static void malloc_inv(int ind, int cal, int p1)
{
	clock_t tm, tf;
	void *ptr;
	double btm = 0;
	double btf = 0;
	clock_t min_tm = 0;
	clock_t max_tm = 0;
	clock_t min_tf = 0;
	clock_t max_tf = 0;

	unsigned long size = 1ull << ind;

	if (p1)
		size++;

	for (int i = 0; i <  NSTAT; i++) {

		if (cal == 2) {
			tm = clock();
			ptr = alloca(size * sizeof(void));
			tm = clock() - tm;
		} else if (cal == 1) {
			tm = clock();
			ptr = calloc(size, sizeof(void));
			tm = clock() - tm;
		} else {
			tm = clock();
			ptr = malloc(size * sizeof(void));
			tm = clock() - tm;
		}

		if (ptr == NULL) {
			printf("%2d %22lu: Memory not allocated.\n", ind, size);
			return;
		}

		btm += ((double)tm);
		if (tm < min_tm)
			min_tm = tm;
		if (tm > max_tm)
			max_tm = tm;

		if (cal != 2) {
			tf = clock();
			free(ptr);
			tf = clock() - tf;
			btf += tf;

			if (tf < min_tf)
				min_tf = tf;
			if (tf > max_tf)
				max_tf = tf;
		}

	}
	printf("%2d %22lu %10ld %10f %10ld %10ld %10f %10ld\n",
	ind, size, min_tm, btm/NSTAT, max_tm, min_tf, btf/NSTAT, max_tf);
}

int main(int argc, char *argv[])
{
	int i;

	printf("Current system has %ld clocks per one second\n",
						CLOCKS_PER_SEC);

	printf("************************\n");
	printf("* malloc function, 2^n *\n");
	printf("************************\n");
	for (i = 0; i < NVALUE; i++)
		malloc_inv(i, 0, 0);

	printf("************************\n");
	printf("* calloc function, 2^n *\n");
	printf("************************\n");
	for (i = 0; i < NVALUE; i++)
		malloc_inv(i, 1, 0);

// Segmentation fault:
	printf("alloca function, 2^n\n");
	for (i = 0; i < 19; i++)
		malloc_inv(i, 2, 0);

	printf("***************************\n");
	printf("* malloc function, 2^n+1  *\n");
	printf("***************************\n");
	for (i = 0; i < NVALUE; i++)
		malloc_inv(i, 0, 1);

	printf("****************************\n");
	printf("* calloc function, 2^n+1   *\n");
	printf("****************************\n");
	for (i = 0; i < NVALUE; i++)
		malloc_inv(i, 1, 1);

// Segmentation fault:
	printf("alloca function, 2^n+1\n");
	for (i = 0; i < 19; i++)
		malloc_inv(i, 2, 1);

	return 0;
}

