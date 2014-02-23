#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

CC   = gcc
OBJ  = stkclient.o stkui.o stkchat.o stkpacket.o stkbuddy.o stkutil.o
LINKOBJ  = stkclient.o stkui.o stkchat.o stkpacket.o stkbuddy.o stkutil.o
LIBS =  $(shell pkg-config --libs gtk+-2.0) -lgthread-2.0
CFLAGS = -D_LINUX_ $(shell pkg-config --cflags gtk+-2.0)
BIN = stkclient
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before stkclient all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o stkclient $(LIBS)

stkclient.o: stkclient.c
	$(CC) -c stkclient.c -o stkclient.o $(CFLAGS)

stkui.o: stkui.c
	$(CC) -c stkui.c -o stkui.o $(CFLAGS)

stkchat.o: stkchat.c
	$(CC) -c stkchat.c -o stkchat.o $(CFLAGS)

stkpacket.o: stkpacket.c
	$(CC) -c stkpacket.c -o stkpacket.o $(CFLAGS)

stkbuddy.o: stkbuddy.c
	$(CC) -c stkbuddy.c -o stkbuddy.o $(CFLAGS)

stkutil.o: stkutil.c
	$(CC) -c stkutil.c -o stkutil.o $(CFLAGS)

