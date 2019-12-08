obj-m += mod_abs_time.o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules

clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
