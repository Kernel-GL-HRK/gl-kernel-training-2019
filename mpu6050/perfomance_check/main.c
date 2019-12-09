#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILES_NUM 7

const char *check_files[FILES_NUM] = {
    "/sys/class/mpu6050/accel_x",
    "/sys/class/mpu6050/accel_y",
    "/sys/class/mpu6050/accel_z",
    "/sys/class/mpu6050/gyro_x",
    "/sys/class/mpu6050/gyro_y",
    "/sys/class/mpu6050/gyro_z",
    "/sys/class/mpu6050/temperature"
};

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

int check(char *file_path)
{
    struct timespec t1, t2, td;
    int file;
    int ret;
    char tmp[256];

    file = open(file_path, O_RDONLY);

    if(file < 0)
    {
        printf("Cant open file \n");
        return 1;
    }

    t1 = get_timestamp();
    ret = read(file, tmp, sizeof(tmp));
    t2 = get_timestamp();

    if(ret != 0)
    {
        td = sub(t2, t1);
        printf("%s - %ld.%09ld\n", file_path, td.tv_sec, td.tv_nsec);
        close(file);
        return 0;
    }
    else
    {
        printf("Error!!!!");
        close(file);
        return 1;
    }
}

int  main(int argc, char *argv[])
{

    for(int i = 0; i < FILES_NUM; i++)
        check(check_files[i]);
    return 0;
}
