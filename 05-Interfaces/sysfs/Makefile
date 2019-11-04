obj-m += lowercase_conv_module.o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules

clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
