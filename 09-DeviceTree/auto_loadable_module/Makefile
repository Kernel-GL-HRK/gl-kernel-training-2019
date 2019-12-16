obj-m += auto_loadable_module.o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules

clean:
	make -C ${BUILD_KERNEL} M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean
