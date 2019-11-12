#include "stdio.h"
#include "stdlib.h"
#include <time.h>

int main(int argc, char *argv[])
{
time_t t = time(NULL);
struct  tm datetime = *localtime(&t);

printf("Date: %02d/%02d/%04d\n", datetime.tm_mday, datetime.tm_mon+1,
    datetime.tm_year+1900);
printf("Time: %02d:%02d:%02d\n", datetime.tm_hour, datetime.tm_min,
    datetime.tm_sec);

exit(0);
}
