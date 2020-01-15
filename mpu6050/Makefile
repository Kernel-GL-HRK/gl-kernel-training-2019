#
# mpu6050 accelerometer & gyroscope
#
ifneq ($(KERNELRELEASE),)

obj-m := mpu6050.o

else

CROSS_COMPILER ?= arm-linux-gnueabihf-
CURDIR ?= $(PWD)
ARCH = arm

ifeq ($(BUILD_KERNEL),)
	$(error Path to kernel tree - KERNELDIR variable is not defined!)
endif

.PHONY: all clean

all:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) modules

dtbo:
	$(BUILD_KERNEL)scripts/dtc/dtc -I dts -O dtb \
		-o mpu6050.dtbo mpu6050.dtsi

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) clean
	-rm ./mpu6050.dtbo 


endif
