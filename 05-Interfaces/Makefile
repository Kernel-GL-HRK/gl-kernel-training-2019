REMOTE_HOST = linux_4_14
REMOTE_PATH = /home/user/modules
MODULE_NAME = mymodule

obj-m += $(MODULE_NAME).o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules

clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean

install: $(MODULE_NAME).ko
	scp $< $(REMOTE_HOST):$(REMOTE_PATH)
