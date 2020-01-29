NAME = st7735s

ifneq ($(KERNELRELEASE),)

obj-m := $(NAME).o

else

.PHONY: default clean

default: $(NAME).c
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif
