// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  mpu6050_user.c - Test
 *
 *  Copyright (C) 2019 Andrey Pahomov <pahomov.and@gmail.com>
 */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdbool.h>


#define MPU6050_ID 0x68

#define CHECK_SET_BIT(num, bit) (((num) >> bit)&0x1)
#define CHECK_CLR_BIT(num, bit) ((~(num) >> bit)&0x1)
#define SET_BIT(num, bit) (((num) |= 1 << bit))
#define CLR_BIT(num, bit) ((num) &= ~(1 << bit))

enum REG_WHO_AM_I {
	CLKSEL_0 = 0,
	CLKSEL_1 = 1,
	CLKSEL_2 = 2,
	TEMP_DIS = 3,
	CYCLE = 5,
	SLEEP = 6,
	DEVICE_RESET = 7
};

enum REGS_MAP {
	TEMP_OUT_H = 0x41,
	TEMP_OUT_L = 0x42,
	PWR_MGMT_1 = 0x6B,
	WHO_AM_I = 0x75

};



int i2c_fd;
const char *i2c_device = "/dev/i2c-0";
const uint8_t slave_addr = 0x68;

int i2c_read(uint8_t slave_addr, uint8_t reg, uint8_t *result)
{
	uint8_t outbuf[1], inbuf[1];
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data msgset[1];

	if (!i2c_fd)
		i2c_fd = open(i2c_device, O_RDWR);

	msgs[0].addr = slave_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = outbuf;

	msgs[1].addr = slave_addr;
	msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
	msgs[1].len = 1;
	msgs[1].buf = inbuf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 2;

	outbuf[0] = reg;

	inbuf[0] = 0;

	*result = 0;
	if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0) {
		printf("%s error",  __func__);
		return -1;
	}

	*result = inbuf[0];
	return 0;
}

int i2c_write(uint8_t slave_addr, uint8_t reg, uint8_t data)
{
	uint8_t outbuf[2];
	struct i2c_msg msgs[1];
	struct i2c_rdwr_ioctl_data msgset[1];

	if (!i2c_fd)
		i2c_fd = open(i2c_device, O_RDWR);

	outbuf[0] = reg;
	outbuf[1] = data;

	msgs[0].addr = slave_addr;
	msgs[0].flags = 0;
	msgs[0].len = 2;
	msgs[0].buf = outbuf;

	msgset[0].msgs = msgs;
	msgset[0].nmsgs = 1;

	if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0) {
		printf("%s error",  __func__);
		return -1;
	}

	return 0;
}


int main(int argc, char *argv[])
{

	uint8_t ID;
	uint8_t temp_H, temp_L;
	int16_t temp_HL;
	struct timespec ts;
	double T_C, T_F;
	int opt;
	bool verbose, measurement, reset;

	verbose = false;
	measurement = false;
	reset = false;

	i2c_fd = 0;


	while ((opt = getopt(argc, argv, "vmr")) != -1) {
		switch (opt) {
		case 'v':
			verbose = true;
			break;
		case 'm':
			measurement = true;
			break;
		case 'r':
			reset = true;
			break;
		default:
			fprintf(stderr, "Usage: %s [-vm]\n", argv[0]);
			verbose = true;
		}
	}

	i2c_read(slave_addr, WHO_AM_I, &ID);

	if (verbose) {
		printf("WHO_AM_I : 0x%02x\n", ID);
		printf("t(UTC sec)\tT(C)\tT(F)\n");
	}

	if ((ID == MPU6050_ID) && (reset)) {
		uint8_t val;

		i2c_read(slave_addr, PWR_MGMT_1, &val);

		SET_BIT(val, DEVICE_RESET);
		i2c_write(slave_addr, PWR_MGMT_1, val);

		usleep(100000);

		i2c_read(slave_addr, PWR_MGMT_1, &val);
		CLR_BIT(val, SLEEP);
		i2c_write(slave_addr, PWR_MGMT_1, val);

		usleep(100000);

	}


	if ((ID == MPU6050_ID) && (measurement)) {

		i2c_read(slave_addr, TEMP_OUT_L, &temp_L);
		i2c_read(slave_addr, TEMP_OUT_H, &temp_H);

		temp_HL = temp_H << 8 | temp_L;
		T_C = temp_HL/340 + 36.53;
		T_F = T_C*1.8 + 32;

		timespec_get(&ts, TIME_UTC);
		printf("%lu.%09lu\t%.02f\t%.02f\n",
				   ts.tv_sec, ts.tv_nsec, T_C, T_F);
	}



	return 0;
}

