CC = gcc
SRC = mkbootimg.c
OBJ = mkbootimg.o

TARGET = msm8655_surf

BUILD_HOST_ARCH_IS_ARM = $(shell uname -m|grep arm.*)

CFLAGS =  -I../include
CFLAGS += -I../../$(TARGET)
CFLAGS += -c -fno-exceptions -Wno-multichar -fPIC

ifeq "$(BUILD_HOST_ARCH_IS_ARM)" ""
  CFLAGS += -m32
endif

CFLAGS += -fmessage-length=0
CFLAGS += -W -Wall -Wno-unused -Winit-self -Wpointer-arith
CFLAGS += -O2 -g -fno-strict-aliasing
CFLAGS += -DSURF8K

CXX = g++
CXXFLAGS =  -Wl,--no-undefined

ifeq "$(BUILD_HOST_ARCH_IS_ARM)" ""
  CXXFLAGS += -m32
endif

LIBS = ../libmincrypt/libmincrypt.a

.PHONY : all
all : mkbootimg-$(TARGET)

mkbootimg-$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@

.PHONY : clean
clean:
	rm -rf $(OBJ) mkbootimg-$(TARGET)
