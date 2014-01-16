#
# Makefile for STK
#
#  Copyright (C) 2014 SixTeam
#  All Rights Reserved
#

OBJS = stkserver.o packet.o stkuser.o
LIBS = 
EXE = stkserver

$(EXE): $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LIBS)

clean: 
	rm -f *.o *.d
	rm -f $(EXE)
	
#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
