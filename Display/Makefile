#
# mpu6050 accelerometer & gyroscope
#
obj-m := st7735s.o

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-


.PHONY: all clean

#dtbo:
#	dtc -I dts -O dtb -o sun8i-h3-mpu6050.dtbo sun8i-h3-mpu6050.dts

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
