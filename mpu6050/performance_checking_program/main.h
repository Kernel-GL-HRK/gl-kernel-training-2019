/*
* Created on: Nov 17th 2019
* Author's email: bekirbekirov1986@gmail.com
*/
#ifndef MAIN_H_
#define MAIN_H_

#include <time.h>

#define NS 1000000000UL
#define BUF_SIZE 256

#define ACCEL_X_FILE "/sys/class/mpu6050/accel_x"
#define ACCEL_Y_FILE "/sys/class/mpu6050/accel_y"
#define ACCEL_Z_FILE "/sys/class/mpu6050/accel_z"
#define GYRO_X_FILE "/sys/class/mpu6050/gyro_x"
#define GYRO_Y_FILE "/sys/class/mpu6050/gyro_y"
#define GYRO_Z_FILE "/sys/class/mpu6050/gyro_z"
#define TEMP_FILE "/sys/class/mpu6050/temp"

typedef struct {
	struct timespec time_end;
	struct timespec time_start;
} time_struct;

#define DEFAULT_TIME_STRUCT_INIT { {0, 0}, {0, 0} }

unsigned long long time_interval(struct timespec *time_end,
					struct timespec *time_start);
int check_perfomance(const char *sysfs_file_name, time_struct *ptr_data);

#endif /* MAIN_H_ */
