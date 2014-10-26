# Project: sixtalk
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = stkclient.o stkutil.o stkpacket.o stkbuddy.o stkui.o $(RES)
LINKOBJ  = stkclient.o stkutil.o stkpacket.o stkbuddy.o stkui.o $(RES)
LIBS =  -L"C:/App/Dev-Cpp/lib" -lws2_32 -lwinmm -lcomctl32 -mwindows  
INCS =  -I"C:/App/Dev-Cpp/include" 
CXXINCS =  -I"C:/App/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"C:/App/Dev-Cpp/include/c++/3.4.2/backward"  -I"C:/App/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"C:/App/Dev-Cpp/include/c++/3.4.2"  -I"C:/App/Dev-Cpp/include" 
BIN  = sixtalk.exe
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before sixtalk.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "sixtalk.exe" $(LIBS)

stkclient.o: stkclient.c
	$(CC) -c stkclient.c -o stkclient.o $(CFLAGS)

stkutil.o: stkutil.c
	$(CC) -c stkutil.c -o stkutil.o $(CFLAGS)

stkpacket.o: stkpacket.c
	$(CC) -c stkpacket.c -o stkpacket.o $(CFLAGS)

stkbuddy.o: stkbuddy.c
	$(CC) -c stkbuddy.c -o stkbuddy.o $(CFLAGS)

stkui.o: stkui.c
	$(CC) -c stkui.c -o stkui.o $(CFLAGS)
