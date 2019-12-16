PROJECT_NAME=mpu6050_user
#ARCH=arm
CC=../../../toolchain/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SRC=mpu6050_user.c
OBJS=$(SRC:.c=.o)

SSH_ORANGE?=root@192.168.1.108

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)


upload_config:
	ssh-copy-id -i ~/.ssh/id_rsa.pub $(SSH_ORANGE)

upload: $(PROJECT_NAME)
	-scp $< $(SSH_ORANGE):~
