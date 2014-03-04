#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

CC      = gcc
OBJ     = stkserver.o stkui.o stkpacket.o stkuser.o stkgroup.o cJSON.o
LINKOBJ = stkserver.o stkui.o stkpacket.o stkuser.o stkgroup.o cJSON.o
LIBS    = $(shell pkg-config --libs gtk+-2.0) -lgthread-2.0
CFLAGS  = -D_LINUX_ $(shell pkg-config --cflags gtk+-2.0)
BIN     = stkserver
RM      = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

stkserver.o: stkserver.c
	$(CC) -c stkserver.c -o stkserver.o $(CFLAGS)

stkui.o: stkui.c
	$(CC) -c stkui.c -o stkui.o $(CFLAGS)

stkpacket.o: stkpacket.c
	$(CC) -c stkpacket.c -o stkpacket.o $(CFLAGS)

stkuser.o: stkuser.c
	$(CC) -c stkuser.c -o stkuser.o $(CFLAGS)

stkgroup.o: stkgroup.c
	$(CC) -c stkgroup.c -o stkgroup.o $(CFLAGS)

cJSON.o: cJSON.c
	$(CC) -c cJSON.c -o cJSON.o $(CFLAGS)
