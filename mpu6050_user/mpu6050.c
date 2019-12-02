#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/time.h>
#include <time.h>

/* MPU6050 internal registers */
#define WHO_AM_I	0x75
#define TEMP_OUT_H	0x41
#define TEMP_OUT_L	0x42
#define PWR_MGMT_1	0x6b

#define GYRO_ADDR	0x68

//------------------------------------------------------------------------------
int readData(int fd, u_int8_t regAddr, u_int8_t *buf, int len)
{
	// Write memory address again
	write(fd, &regAddr, 1);

	// Read bytes
	if (!read(fd, buf, len)) {
		printf("Failed to read buffer\n");
		return -1;
	}

	//printf("Reg addr: 0x%02X, Read byte: 0x%02X\n", regAddr, buf);
	return 0;
}
//------------------------------------------------------------------------------
int writeData(int fd, u_int8_t regAddr, u_int8_t val)
{
	int w = 0;
	u_int8_t buf[2] = {0};

	buf[0] = regAddr;
	buf[1] = val;

// Write memory address again
	w = write(fd, buf, 2);
	if (w != 2) {
		printf("Failed to write buffer, err: %d\n", w);
		return -1;
	}

	//printf("Reg addr: 0x%02X, Write byte: 0x%02X\n", regAddr, val);
	return 0;
}

//------------------------------------------------------------------------------
int gyro_temp(int i2c_fd, float *temp)
{
	u_int8_t buf[2] = {0};

	if (readData(i2c_fd, TEMP_OUT_H, buf, 2) < 0)
		return -1;

	//printf("Read bytes: 0x%04X\n", buf[1] | (buf[0] << 8));
	//printf("Val: %d\n", (short)(buf[1] | (buf[0] << 8)));

	*temp = (short)(buf[1] | (buf[0] << 8));
	*temp /= 340;
	*temp += 35;

	return 0;
}
//------------------------------------------------------------------------------
int gyro_init(int i2c_fd)
{
	u_int8_t buf;

	if (readData(i2c_fd, WHO_AM_I, &buf, 1) < 0)
		return -1;

	if (buf != GYRO_ADDR)
		return -1;

	usleep(1000);

	// reset
	writeData(i2c_fd, PWR_MGMT_1, 0x80);
	usleep(1000);

	// init
	writeData(i2c_fd, PWR_MGMT_1, 0x00);
	sleep(1);

	//printf("Reg addr: 0x%02X, Read byte: 0x%02X\n", WHO_AM_I, buf);
	return 0;
}
//------------------------------------------------------------------------------
int main(void)
{
	int i2c_fd = 0, ret = 0;
	float temp = 0;
	struct timeval t = {};

	/* open i2c device */
	printf("Open i2c-0...\n");
	i2c_fd = open("/dev/i2c-0", O_RDWR);
	if (i2c_fd < 0) {
		printf("Failed to open the i2c bus\n");
		return EXIT_FAILURE;
	}

	/* set slave address */
	ret = ioctl(i2c_fd, I2C_SLAVE, GYRO_ADDR);
	if (ret < 0) {
		printf("Failed to acquire bus access and/or talk to slave\n");
		return EXIT_FAILURE;
	}

	/* gyro init */
	printf("Gyro init...\n");
	ret = gyro_init(i2c_fd);
	if (ret < 0) {
		printf("Gyro init error!\n");
		return EXIT_FAILURE;
	}

	printf("Reading temp...\n");
	while (1) {
		/* gyro read temp */
		ret = gyro_temp(i2c_fd, &temp);
		if (ret < 0) {
			printf("Gyro read temp error!\n");
			return EXIT_FAILURE;
		}

		ret = gettimeofday(&t, NULL);
		if (ret < 0)
			return EXIT_FAILURE;

		printf("[%ld.%06ld] Temp: %02.3f C\n",
				t.tv_sec, t.tv_usec, temp);
		printf("[%ld.%06ld] Temp: %02.3f F\n",
				t.tv_sec, t.tv_usec, temp*9/5+32);
		printf("[%ld.%06ld] --------------\n",
				t.tv_sec, t.tv_usec);

		sleep(1); // 1s
	}
}
//------------------------------------------------------------------------------
