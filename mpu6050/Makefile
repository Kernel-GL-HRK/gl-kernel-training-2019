obj-m += mpu6050.o

all:
	make -C $(BUILD_KERNEL) M=$(PWD) CROSS_COMPILE=$(CROSS_COMPILER) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) CROSS_COMPILE=$(CROSS_COMPILER) clean
