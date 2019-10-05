.PHONY: all clean

all:game

CC = gcc
CFLAGS += -O0 -g3 -Wall -MD -fmessage-length=0 -MP
CPPFLAGS += -I"./" 

LIB = -lpthread

DIRSRC = .
DIRSRC_SUFFIX = $(addsuffix /,$(DIRSRC))

DIRSRC_SEARCH = $(addsuffix *.c,$(DIRSRC_SUFFIX))
VPATH = $(DIRSRC)

SRC = $(wildcard $(DIRSRC_SEARCH))

game: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPFLAGS) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CPPFLAGS) $(LIB)

include $(wildcard *.d)

clean:
	rm -rf *.o *.d
