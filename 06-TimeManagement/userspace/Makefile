PROJECT_NAME=main
CC=gcc
CFLAGS= -static -c -g -Wall
LDFLAGS=
SRC=$(PROJECT_NAME).c
OBJS=$(SRC:.c=.o)

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@


.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)
\ No newline at end of file
