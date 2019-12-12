obj-m += test_module.o
KERNEL_DIR :=  /home/ghost/Documents/global_logic/gl-kernel-training-2019/different_version_linux/5.3
COMPILE_FLAGS += CROSS_COMPILE=arm-linux-gnueabihf- CROSS=arm-linux-gnueabihf- ARCH=arm

all:
		make -C $(KERNEL_DIR) M=$(PWD) $(COMPILE_FLAGS) modules
clean:
		make -C $(KERNEL_DIR) M=$(PWD) $(COMPILE_FLAGS) clean

