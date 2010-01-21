CC = gcc
SRC = mkbootimg.c
OBJ = mkbootimg.o

CFLAGS =  -I../include
CFLAGS += -I../../qsd8250_surf
CFLAGS += -c -fno-exceptions -Wno-multichar -m32 -fPIC 
#CFLAGS += -include ../../../not-gerrit/donut/system/core/include/arch/linux-x86/AndroidConfig.h
#CFLAGS += -DANDROID
CFLAGS += -fmessage-length=0 
CFLAGS += -W -Wall -Wno-unused -Winit-self -Wpointer-arith
CFLAGS += -O2 -g -fno-strict-aliasing
#CFLAGS += -DNDEBUG -UDEBUG
CFLAGS += -DSURF8K

CXX = g++
CXXFLAGS =  -Wl,--no-undefined -m32
#CXXFLAGS += -Wl,-rpath-link=../../../not-gerrit/donut/out/target/product/qsd8250_surf/obj/lib
#CXXFLAGS += -Wl,-rpath,\$ORIGIN/../lib -Lout/host/linux-x86/obj/lib

LIBS = ../libmincrypt/libmincrypt.a

.PHONY : all
all : mkbootimg-qsd8250_surf

mkbootimg-qsd8250_surf :
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@
