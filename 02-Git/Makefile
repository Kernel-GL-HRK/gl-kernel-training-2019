.PHONY: all clean
all:rps

CC = gcc
CFLAGS += -O0 -g3 -Wall -MD -fmessage-length=0 -MP
CPPFLAGS += -I"./" 
DIRSRC = ./
DIRSRC_SEARCH = $(addsuffix *.c,$(DIRSRC))
SRC = $(wildcard $(DIRSRC_SEARCH))

rps: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@ $(CPPFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CPPFLAGS)
include $(wildcard *.d)

clean:
	rm -rf *.o *.d

