#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

CC      = gcc
OBJ     = stkclient.o stkui.o stkchat.o stkgchat.o stkpacket.o stkbuddy.o stkutil.o stkwav.o
LINKOBJ = stkclient.o stkui.o stkchat.o stkgchat.o stkpacket.o stkbuddy.o stkutil.o stkwav.o
LIBS    = $(shell pkg-config --libs gtk+-2.0) -lgthread-2.0
CFLAGS  = -D_LINUX_ $(shell pkg-config --cflags gtk+-2.0)
BIN     = stkclient
RM      = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

stkclient.o: stkclient.c
	$(CC) -c stkclient.c -o stkclient.o $(CFLAGS)

stkui.o: stkui.c
	$(CC) -c stkui.c -o stkui.o $(CFLAGS)

stkchat.o: stkchat.c
	$(CC) -c stkchat.c -o stkchat.o $(CFLAGS)

stkgchat.o: stkgchat.c
	$(CC) -c stkgchat.c -o stkgchat.o $(CFLAGS)

stkpacket.o: stkpacket.c
	$(CC) -c stkpacket.c -o stkpacket.o $(CFLAGS)

stkbuddy.o: stkbuddy.c
	$(CC) -c stkbuddy.c -o stkbuddy.o $(CFLAGS)

stkutil.o: stkutil.c
	$(CC) -c stkutil.c -o stkutil.o $(CFLAGS)

stkwav.o: stkwav.c
	$(CC) -c stkwav.c -o stkwav.o $(CFLAGS)
