SHELL=/bin/bash

CC = gcc
CFLAGS = -c -g
LDFLAGS = -W

PROGRAM_NAME = lab3

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
BINS := $(SRCS:%.c=%~)

.INTERMEDIATE: $(OBJS)

all: remove_bin link

remove_bin:
	rm -rvf $(PROGRAM_NAME)

link: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROGRAM_NAME)

%.o: %.c
	@echo "Compiling "$<" to object file "$@"..."
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rvf $(PROGRAM_NAME)
