PROJECT_NAME=program_06
CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SRC=program_06.c
OBJS=$(SRC:.c=.o)

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)
