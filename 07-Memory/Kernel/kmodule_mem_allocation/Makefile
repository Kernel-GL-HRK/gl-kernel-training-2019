obj-m += kmodule_mem_allocation.o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules

clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean

instal:
	scp kmodule_mem_allocation.ko myLinux:/home/user/anton

reinstal:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
	make -C ${BUILD_KERNEL} M=$(PWD) modules
	scp kmodule_mem_allocation.ko myLinux:/home/user/anton
		
