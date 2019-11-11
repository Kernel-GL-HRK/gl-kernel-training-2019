SYSTEM ?=

ifeq ($(SYSTEM), qemu)
	NAMELINUX = QemuLinux
	QDIR = /home/dmitry
endif

NAME = myprog

$(NAME): $(NAME).c
ifeq ($(SYSTEM), qemu)
	gcc $< -o $@ -march=i386 -mtune=i686 -m32 -static
else
	gcc $< -o $@
endif

copyprog: $(NAME)
	scp $< $(NAMELINUX):$(QDIR)

copysshid:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(NAMELINUX)

clean:
	@rm -f $(NAME)
