NAME = mpu6050

ifneq ($(KERNELRELEASE),)

obj-m := $(NAME).o

else

NAMELINUX ?= root@192.168.0.130
MODULEDIR ?= /lib/modules/4.19.83-sunxi/kernel/drivers/iio/accel
KERNELDIR ?= /home/dmitry/GL/orange-pi-4.19.83
DTSDIR ?= $(PWD)/dtsi

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-

.PHONY: all clean

all: $(NAME).c
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

dtbo:
	$(KERNELDIR)/scripts/dtc/dtc -I dts -O dtb \
		-o $(DTSDIR)/$(NAME).dtbo \
		$(DTSDIR)/$(NAME).dtsi

copymod: $(NAME).ko
	scp $< $(NAMELINUX):$(MODULEDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

copydtbo:
	scp $(PWD)/dtsi/$(NAME).dtbo \
		$(NAMELINUX):/boot/overlay-user

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
	-rm $(DTSDIR)/$(NAME).dtbo

endif
