obj-m+=mymodule.o

BUILD_KERNEL:= /usr/src/linux-headers-$(shell uname -r)
all:
	make -C $(BUILD_KERNEL) M=$(shell pwd) modules
	
clean:
	make -C $(BUILD_KERNEL) M=$(shell pwd) clean
