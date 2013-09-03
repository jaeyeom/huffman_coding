# Makefile for the Engineering Mathmatics
# Homework 2nd: Huffman Coding

CC = gcc
CFLAGS = -Wall -O2

SHAREDSRCS = huff.c heap.c
MAINSRCS = huffenc.c huffdec.c frqdump.c
SRCS = $(SHAREDSRCS) $(MAINSRCS)

SHAREDOBJS = $(SHAREDSRCS:.c=.o)
MAINOBJS = $(MAINSRCS:.c=.o)
OBJS = $(SHAREDOBJS) $(MAINOBJS)

TARGETS = $(MAINOBJS:.o=)

FILES = $(OBJS) $(TARGETS)

all: $(FILES)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.o:
	$(CC) $(CFLAGS) -o $@ $< $(SHAREDOBJS)

clean:
	rm -f $(FILES) *~
