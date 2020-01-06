BUILD_KERNEL ?= ~/gl/build/cache/sources/linux-mainline/orange-pi-5.3/
CROSS_COMPILER ?= arm-linux-gnueabihf-
CCFLAGS ?= -C
ARCH = arm

obj-m += fake_module.o

all: fake_module.c
	make $(CCFLAGS) $(BUILD_KERNEL) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) modules

dtbo:
	$(BUILD_KERNEL)scripts/dtc/dtc -I dts -O dtb \
		-o fake_module.dtbo \
			fake_module.dtsi

clean:	
	make $(CCFLAGS) $(BUILD_KERNEL) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILER) clean
	-rm ./fake_module.dtbo