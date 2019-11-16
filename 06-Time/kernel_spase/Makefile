#obj-m += module_procfs.o
obj-m += module_sysfs_time.o

#BUILD_KERNEL :=  /home/vadim/work/ssd_250/gl-kernel-training-2019/linux-stable/
BUILD_KERNEL:= /usr/src/linux-headers-$(shell uname -r)

all:
		make -C $(BUILD_KERNEL) M=$(shell pwd) modules
clean:
		make -C $(BUILD_KERNEL) M=$(shell pwd) $(COMPILE_FLAGS) clean
