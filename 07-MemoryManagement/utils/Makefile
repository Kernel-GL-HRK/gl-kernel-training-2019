PROJECT_NAME=program_07
CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SRC=program_07.c
OBJS=$(SRC:.c=.o)

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)
