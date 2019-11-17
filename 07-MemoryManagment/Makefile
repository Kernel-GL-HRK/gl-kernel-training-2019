obj-m += memory_managment_module.o

KERNEL_DIR :=  /home/bekir/Work/LinuxKernelCamp/linux/linux-stable

all:
		make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
		make -C $(KERNEL_DIR) M=$(PWD) $(COMPILE_FLAGS) clean
install:
		make -C $(KERNEL_DIR) M=$(PWD) modules_install INSTALL_MOD_PATH=$(PWD)/build_modules
