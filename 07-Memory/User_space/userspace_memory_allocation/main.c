#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

char *pArr;

enum ALLOC_TYPE_e {
	TYPE_MALLOC = 0,
	TYPE_CALLOC = 1,
	TYPE_ALLOCA = 2
};

struct outResult_t {
	unsigned int allocAvrTime;
	unsigned int allocMinTime;
	unsigned int allocMaxTime;
	unsigned int freeAvrTime;
	unsigned int freeMinTime;
	unsigned int freeMaxTime;
};

unsigned int calcNsTimeDiff(struct timespec *tStart, struct timespec *tStop)
{
	return 1000000000 * (tStop->tv_sec - tStart->tv_sec)
	+ (tStop->tv_nsec - tStart->tv_nsec);
}

int getTimeMemoryAllocation(void *pArr, unsigned int size, int type,
				struct outResult_t *pResult)
{
	struct timespec tStart, tStop;
	int allocError = 0;

	unsigned int allocTimeSum = 0;
	unsigned int freeTimeSum = 0;

	pResult->allocMinTime = 999999999;
	pResult->allocMaxTime = 0;
	pResult->freeMinTime = 999999999;
	pResult->freeMaxTime = 0;

	int i;

	for (i = 0; i < 5; i++) {
		switch (type) {
		case TYPE_MALLOC:
			clock_gettime(CLOCK_REALTIME, &tStart);
			pArr = malloc(size);
			clock_gettime(CLOCK_REALTIME, &tStop);
			break;
		case TYPE_CALLOC:
			clock_gettime(CLOCK_REALTIME, &tStart);
			pArr = calloc(1, size);
			clock_gettime(CLOCK_REALTIME, &tStop);
			break;
		case TYPE_ALLOCA:
			clock_gettime(CLOCK_REALTIME, &tStart);
			pArr = alloca(size);
			clock_gettime(CLOCK_REALTIME, &tStop);
			break;
		}

		if (pArr != NULL) {
			unsigned int currTimeDiff =
				calcNsTimeDiff(&tStart, &tStop);

			allocTimeSum += currTimeDiff;
			if (currTimeDiff <= pResult->allocMinTime)
				pResult->allocMinTime = currTimeDiff;
			if (currTimeDiff >= pResult->allocMaxTime)
				pResult->allocMaxTime = currTimeDiff;

			// free
			if (type != TYPE_ALLOCA) {
				clock_gettime(CLOCK_REALTIME, &tStart);
				free(pArr);
				clock_gettime(CLOCK_REALTIME, &tStop);

				unsigned int currTimeDiff =
				calcNsTimeDiff(&tStart, &tStop);

				freeTimeSum += currTimeDiff;
				if (currTimeDiff <= pResult->freeMinTime)
					pResult->freeMinTime = currTimeDiff;
				if (currTimeDiff >= pResult->freeMaxTime)
					pResult->freeMaxTime = currTimeDiff;
			} else {
				pResult->freeMinTime = 0;
				pResult->freeMaxTime = 0;
			}

		} else {

			allocError = 1;
			return 0;
		}
	}

	if (allocError == 0) {
		pResult->allocAvrTime = (unsigned int)(allocTimeSum/5.);
		pResult->freeAvrTime = (unsigned int)(freeTimeSum/5.);
		return 1;
	} else
		return 0;
}

void showTestMemoryAllocationTable(int type)
{
	switch (type) {
	case TYPE_MALLOC:
		printf(" MALLOC FUNCTION\n");
		break;
	case TYPE_CALLOC:
		printf(" CALLOC FUNCTION\n");
		break;
	case TYPE_ALLOCA:
		printf(" ALLOCA FUNCTION\n");
		break;
	}

	printf("---------------------------------------------");
	printf("-------------------------------------------\n");

	printf("|   bufer size | alloc min | alloc avr | alloc max ");
	printf("|  free min |  free avr |  free max |\n");

	printf("|        bytes |        ns |        ns |        ns ");
	printf("|        ns |        ns |        ns |\n");

	printf("---------------------------------------------");
	printf("-------------------------------------------\n");

	unsigned int bufSize = 1;
	struct outResult_t result;
	int i;

	for (i = 0; i < 32; i++) {
		if (getTimeMemoryAllocation(pArr, bufSize, type, &result)) {
			printf("|%10u    |%10u |%10u |%10u |",
				bufSize, result.allocMinTime,
				result.allocAvrTime,
				result.allocMaxTime);
			printf("%10u |%10u |%10u |\n", result.freeMinTime,
				result.freeAvrTime, result.freeMaxTime);
		} else {
			printf("Allocation error on buff size = %u", bufSize);
		}

		if (getTimeMemoryAllocation(pArr, bufSize, type, &result)) {
			printf("|%10u + 1|%10u |%10u |%10u |",
				bufSize, result.allocMinTime,
				result.allocAvrTime,
				result.allocMaxTime);
			printf("%10u |%10u |%10u |\n", result.freeMinTime,
				result.freeAvrTime, result.freeMaxTime);
		} else {
			printf("Allocation error on buff size = %u+1",
				bufSize);
		}

		bufSize <<= 1;
	}
	printf("---------------------------------------------");
	printf("-------------------------------------------\n");
}

int main(void)
{
	// show tasting memory allocation tables
	showTestMemoryAllocationTable(TYPE_MALLOC);
	showTestMemoryAllocationTable(TYPE_CALLOC);
	showTestMemoryAllocationTable(TYPE_ALLOCA);

	return 0;
}
