obj-m += mpu6050_overlay_test.o

KERNEL_DIR :=  /home/bekir/Work/LinuxKernelCamp/linux/linux-stable

COMPILE_FLAGS += CROSS_COMPILE=arm-linux-gnueabihf- CROSS=arm-linux-gnueabihf- ARCH=arm

all:
		make -C $(KERNEL_DIR) M=$(PWD) $(COMPILE_FLAGS) modules
clean:
		make -C $(KERNEL_DIR) M=$(PWD) $(COMPILE_FLAGS) clean
install:
		make -C $(KERNEL_DIR) M=$(PWD) modules_install INSTALL_MOD_PATH=$(PWD)/build_modules
