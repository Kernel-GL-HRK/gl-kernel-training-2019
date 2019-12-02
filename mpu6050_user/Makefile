NAMELINUX ?= dmitry@192.168.0.130
QDIR = /home/dmitry
CROSS_COMPILE = arm-linux-gnueabihf-

NAME = mpu6050

$(NAME): $(NAME).c
	$(CROSS_COMPILE)gcc $< -o $@

copyprog: $(NAME)
	scp $< $(NAMELINUX):$(QDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

clean:
	@rm -f $(NAME)
