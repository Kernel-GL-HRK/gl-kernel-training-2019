.PHONY: all clean

all:time

CC = gcc
CFLAGS += -m32 -static -O0 -g3 -Wall -MD -fmessage-length=0 -MP
CPPFLAGS += -I"./"

DIRSRC = .
DIRSRC_SUFFIX = $(addsuffix /,$(DIRSRC))

DIRSRC_SEARCH = $(addsuffix *.c,$(DIRSRC_SUFFIX))
VPATH = $(DIRSRC)

SRC = $(wildcard $(DIRSRC_SEARCH))

time: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPFLAGS) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CPPFLAGS) $(LIB)

include $(wildcard *.d)

clean:
	rm -rf *.o *.d
