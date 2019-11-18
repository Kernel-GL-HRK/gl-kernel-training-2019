obj-m+=module_07.o
ccflags-y := -Wall

BUILD_KERNEL?=/lib/modules/$(shell uname -r)/build/
#ARCH?=i386

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules
clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
