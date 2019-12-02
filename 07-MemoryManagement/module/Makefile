SYSTEM ?=
NAMELINUX = QemuLinux
QDIR = /home/dmitry
NAME = mymodule

obj-m += $(NAME).o

ifeq ($(SYSTEM), qemu)
	KDIR = /home/dmitry/GL/kernel_v4.13
else
	KDIR = /lib/modules/$(shell uname -r)/build
endif

all: $(NAME).c
	make -C $(KDIR) M=$(PWD) modules

copymod: $(NAME).ko
	scp $< $(NAMELINUX):$(QDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

clean:
	make -C $(KDIR) M=$(PWD) clean

