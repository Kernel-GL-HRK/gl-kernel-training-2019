#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <time.h>
#include <math.h>

struct timespec get_timestamp(void)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ts;
}

struct timespec sub(struct timespec arg1, struct timespec arg2)
{
	struct timespec ret;
	ret.tv_sec = arg1.tv_sec - arg2.tv_sec;
	ret.tv_nsec = arg1.tv_nsec - arg2.tv_nsec;
	if(ret.tv_nsec < 0)
	{
		ret.tv_nsec += 1000000000;
		ret.tv_sec -= 1;
	}
	return ret;
}

static const char *label[] = {"malloc", "calloc", "alloca"};

void testMalloc(void)
{
	struct timespec t1, t2;
	void *ptr;

	for (int i = 0; i <= 64; i++) {
		for(int oversize = 0; oversize <= 1; oversize++) {

			size_t size = (1 << i) + oversize;

			t1 = get_timestamp();
			ptr = malloc(size);
			t2 = get_timestamp();

			if( ptr == NULL )
			{
				printf("ptr = NULL\n");
				return;
			}
			free(ptr);

			struct timespec dt = sub(t2, t1);
			printf("malloc:2^%d+%01d = %ld.%09ld\n", i, oversize, dt.tv_sec, dt.tv_nsec);
		}
	}
}

void testCalloc(void)
{
	struct timespec t1, t2;
	void *ptr;

	for (int i = 0; i <= 64; i++) {
		for(int oversize = 0; oversize <= 1; oversize++) {

			size_t size = (1 << i) + oversize;

			t1 = get_timestamp();
			ptr = calloc(1, size);
			t2 = get_timestamp();

			if( ptr == NULL )
			{
				printf("ptr = NULL\n");
				return;
			}
			free(ptr);

			struct timespec dt = sub(t2, t1);
			printf("calloc:2^%d+%01d = %ld.%09ld\n", i, oversize, dt.tv_sec, dt.tv_nsec);
		}
	}
}

void testAlloca(void)
{
	struct timespec t1, t2;
	void *ptr;

	for (int i = 0; i <= 64; i++) {
		for(int oversize = 0; oversize <= 1; oversize++) {

			size_t size = (1 << i) + oversize;

			t1 = get_timestamp();
			ptr = alloca(size);
			t2 = get_timestamp();

			struct timespec dt = sub(t2, t1);
			printf("alloca:2^%d+%01d = %ld.%09ld\n", i, oversize, dt.tv_sec, dt.tv_nsec);
		}
	}
}


int  main(int argc, char *argv[])
{
	testMalloc();
	testCalloc();
	testAlloca();
	return 0;
}
