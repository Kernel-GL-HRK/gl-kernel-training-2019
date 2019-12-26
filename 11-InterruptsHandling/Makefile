#
# mpu6050 accelerometer & gyroscope
#
obj-m := mpu6050.o

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-


.PHONY: all clean


all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

dtbo:
	$(KERNELDIR)/scripts/dtc/dtc -I dts -O dtb -o sun8i-h3-mpu6050.dtbo sun8i-h3-mpu6050.dtsi

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
