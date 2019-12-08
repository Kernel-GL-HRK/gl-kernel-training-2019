MODULE_NAME=mpu6050
obj-m :=$(MODULE_NAME).o
ccflags-y := -Wall

PWD=$(shell pwd)
BUILD_KERNEL?=$(PWD)/../../../build/kernel_opi
KERNEL_VERSION?=5.4.0-rc1-00258-g2d00aee21a5d
DTC=$(BUILD_KERNEL)/scripts/dtc/dtc
export CROSS_COMPILE?=$(PWD)/../../../toolchain/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH?=arm

DT_SRCS=$(wildcard *.dts)
DT_OBJS=$(DT_SRCS:.dts=.dtbo)

SSH_ORANGE?=root@192.168.1.108


.PHONY: all clean

all: $(DT_OBJS)
	$(MAKE) -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(PWD) clean
	-rm ./*.dtbo


$(DT_OBJS): $(DT_SRCS)
	$(DTC) -@ -I dts -O dtb -o $@ $<


upload_config:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(SSH_ORANGE)
	echo $(KERNEL_VERSION)
	-ssh $(SSH_ORANGE) '[ ! -d "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/" ] && mkdir -p "/lib/modules/$(KERNEL_VERSION)/kernel/drivers/"'

upload_dtso: $(DT_OBJS)
	-scp $< $(SSH_ORANGE):/boot/dtb/overlay/

upload_module: $(MODULE_NAME).ko
	-scp $< $(SSH_ORANGE):/lib/modules/$(KERNEL_VERSION)/kernel/drivers/
	-ssh $(SSH_ORANGE) 'depmod'

upload: upload_dtso upload_module
