#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>


#define INPUT_ATTR "/sys/class/my_time_mod/absolute-time"

struct timeval t;
int ret, fd;
char str[30];

int main(void)
{
	fd = open(INPUT_ATTR, O_RDONLY);

	if (fd < 0) {
		fprintf(stderr, "Error opening %s for reading\n",
			INPUT_ATTR);
		exit(EXIT_FAILURE);
	}

	ret = read(fd, str, sizeof(str));
	if (ret < 0) {
		fprintf(stderr, "Read error %s\n", INPUT_ATTR);
		exit(EXIT_FAILURE);
	}

	ret = gettimeofday(&t, NULL);
	if (ret < 0)
		exit(EXIT_FAILURE);

	printf("User-space get absolute time: %ld.%06ld\n",
		t.tv_sec, t.tv_usec);
	printf("Kernel-space get absolute time: %s\n", str);

	close(fd);
	return EXIT_SUCCESS;
}
