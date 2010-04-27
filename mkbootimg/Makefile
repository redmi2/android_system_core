CC = gcc
SRC = mkbootimg.c
OBJ = mkbootimg.o

TARGET = msm7630_surf

CFLAGS =  -I../include
CFLAGS += -I../../$(TARGET)
CFLAGS += -c -fno-exceptions -Wno-multichar -m32 -fPIC
CFLAGS += -fmessage-length=0
CFLAGS += -W -Wall -Wno-unused -Winit-self -Wpointer-arith
CFLAGS += -O2 -g -fno-strict-aliasing
CFLAGS += -DSURF8K

CXX = g++
CXXFLAGS =  -Wl,--no-undefined -m32

LIBS = ../libmincrypt/libmincrypt.a

.PHONY : all
all : mkbootimg-$(TARGET)

mkbootimg-$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@

.PHONY : clean
clean:
	rm -rf $(OBJ) mkbootimg-$(TARGET)
