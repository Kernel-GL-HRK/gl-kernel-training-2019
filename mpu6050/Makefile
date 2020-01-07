#
# mpu6050 accelerometer & gyroscope
#
ifneq ($(KERNELRELEASE),)

obj-m := mpu6050.o

else

#BUILD_KERNEL ?= ~/gl/build/cache/sources/linux-mainline/orange-pi-5.3/
CROSS_COMPILER ?= arm-linux-gnueabihf-
CURDIR ?= $(PWD)
ARCH = arm

ifeq ($(BUILD_KERNEL),)
	$(error Path to kernel tree - KERNELDIR variable is not defined!)
endif

.PHONY: all clean

all:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) modules

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) clean


endif
