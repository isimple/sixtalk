#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

CC      = gcc
OBJ     = stkclient.o stkui.o stkpacket.o stkbuddy.o
LINKOBJ = stkclient.o stkui.o stkpacket.o stkbuddy.o
LIBS    = 
CFLAGS  =
BIN     = stkclient
RM      = rm -f

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

stkpacket.o: stkpacket.c
	$(CC) -c stkpacket.c -o stkpacket.o $(CFLAGS)

stkbuddy.o: stkbuddy.c
	$(CC) -c stkbuddy.c -o stkbuddy.o $(CFLAGS)

