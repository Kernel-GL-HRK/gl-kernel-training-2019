NAMELINUX = root@192.168.0.130
QDIR = /lib/modules/4.19.83-sunxi/kernel/drivers/iio/accel
NAME = mpu6050
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
		-o $(PWD)/dtsi/$(NAME).dtbo \
		$(PWD)/dtsi/$(NAME).dtsi

prog: test-mod.c
	$(COMPILER)gcc $< -o $@

copymod: $(NAME).ko
	scp $< $(NAMELINUX):$(QDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

copydtbo:
	scp $(PWD)/dtsi/$(NAME).dtbo \
		$(NAMELINUX):/boot/overlay-user

copyprog: prog
	scp $< $(NAMELINUX):/home/dmitry

clean:
	make $(CCFLAGS) $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) clean
	-rm ./dtsi/$(NAME).dtbo
	-rm prog
