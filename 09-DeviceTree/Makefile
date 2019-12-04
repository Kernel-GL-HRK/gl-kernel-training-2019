NAMELINUX = root@192.168.0.130
QDIR = /lib/modules/4.19.83-sunxi/kernel/drivers/gpio/
NAME = fake_module
KDIR = /home/dmitry/GL/orange-pi-4.19.83
ARCH = arm
CCFLAGS = -C
COMPILER = arm-linux-gnueabihf-
PWD = $(shell pwd)

obj-m += $(NAME).o

all: $(NAME).c
	make $(CCFLAGS) $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) modules

dtbo:
	~/GL/orange-pi-4.19.83/scripts/dtc/dtc -I dts -O dtb \
		-o ~/GL/gl-kernel-training-2019/09-DeviceTree/dtsi/fake_module.dtbo \
		~/GL/gl-kernel-training-2019/09-DeviceTree/dtsi/fake_module.dtsi

copymod: $(NAME).ko
	scp $< $(NAMELINUX):$(QDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

copydtbo:
	scp ~/GL/gl-kernel-training-2019/09-DeviceTree/dtsi/fake_module.dtbo \
		$(NAMELINUX):/boot/overlay-user

clean:
	make $(CCFLAGS) $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) clean
	-rm ./dtsi/fake_module.dtbo
