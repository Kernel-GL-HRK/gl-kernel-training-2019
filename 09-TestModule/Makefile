obj-m += test_module.o

BUILD_KERNEL :=  /home/vadim/work/ssd_250/gl-kernel-training-2019/build_kernel/

COMPILE_FLAGS += CROSS_COMPILE=arm-linux-gnueabihf- ARCH=arm

all:
		make -C $(BUILD_KERNEL) M=$(shell pwd) $(COMPILE_FLAGS) modules
clean:
		make -C $(BUILD_KERNEL) M=$(shell pwd) $(COMPILE_FLAGS) clean
