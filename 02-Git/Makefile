PROJECT_NAME=game_rps
CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SRC=main.c
OBJS=$(SRC:.c=.o)

all: $(SRC) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(PROJECT_NAME)