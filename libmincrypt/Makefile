AR = ar
CC = gcc

SRC = rsa.c sha.c
OBJ = rsa.o sha.o

BUILD_HOST_ARCH_IS_ARM = $(shell uname -m|grep arm.*)

CFLAGS =  -I../include

CFLAGS += -c -fno-exceptions -Wno-multichar -fPIC

ifeq "$(BUILD_HOST_ARCH_IS_ARM)" ""
  CFLAGS += -m32
endif

CFLAGS += -fmessage-length=0
CFLAGS += -W -Wall -Wno-unused -Winit-self -Wpointer-arith
CFLAGS += -O2 -g -fno-strict-aliasing
CFLAGS += -DSURF8K

ARFLAGS = crs

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

libmincrypt.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

.PHONY : clean
clean:
	rm -rf $(OBJ) libmincrypt.a
