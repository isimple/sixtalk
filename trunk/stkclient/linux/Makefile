#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

CC = gcc
OBJS = stkclient.o stkpacket.o stkui.o stkbuddy.o
LIBS = 
CFLAGS =
BIN = stkclient
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before stkclient all-after


clean: clean-custom
        ${RM} $(OBJ) $(BIN)

$(BIN): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LIBS)

