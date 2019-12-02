PROJECT_NAME=mpu6050_user
CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SRC=mpu6050_user.c
OBJS=$(SRC:.c=.o)

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)
