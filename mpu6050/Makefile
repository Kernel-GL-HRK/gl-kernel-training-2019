obj-m := mpu6050.o
ccflags-y := -Wall

PWD=$(shell pwd)
BUILD_KERNEL?=$(PWD)/../../build/kernel_opi
KERNEL_VERSION?=5.4.0-rc1-00258-g2d00aee21a5d
DTC=$(BUILD_KERNEL)/scripts/dtc/dtc
export CROSS_COMPILE?=$(PWD)/../../toolchain/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH?=arm

DT_SRCS=$(wildcard *.dts)
DT_OBJS=$(DT_SRCS:.dts=.dtbo)

SSH_ORANGE?=root@192.168.1.108


.PHONY: all clean

all:
	$(MAKE) -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(PWD) clean
